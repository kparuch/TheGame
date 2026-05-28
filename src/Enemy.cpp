#include "Player.h"
#include "Enemy.h"
#include "Crate.h"
#include "Pickup.h"
#include "Wall.h"
#include <map>
#include <algorithm>
#include "Bomb.h"
#include <cmath>
#include "ExplosionArea.h"



struct GridPoint {
	int x, y;
	bool operator==(const GridPoint& other) const {
		return x == other.x && y == other.y;
	}
	bool operator<(const GridPoint&other ) const {
		return x != other.x ? x < other.x : y < other.y;
	}
};


Enemy::Enemy(float x, float y, const sf::Texture& texture, const sf::Texture& bombTex, const sf::Texture& expTex, float visionRange, const sf::SoundBuffer& bombSound)
	:sprite(texture), currentEnemyState(EnemyState::Idle), _bombTex(bombTex), _expTex(expTex), _visionRange(visionRange), bombSoundBuf(bombSound)
{
	sprite.setPosition({ x, y });
	sprite.setScale({ 0.18f, 0.18f });
	_currentBombStats.rangeDown = 2;
	_currentBombStats.rangeUp = 2;
	_currentBombStats.rangeLeft = 2;
	_currentBombStats.rangeRight = 2;
	 frameWidth = texture.getSize().x / 2;
	 frameHeight = texture.getSize().y / 3;
	updateAnimation();
}
void Enemy::decideAction(const std::vector<std::unique_ptr<Entity>>& entities) {
	Player* targetPlayer = nullptr;
	for (const auto& enty : entities) {
		if (Player* p = dynamic_cast<Player*> (enty.get())) {
			targetPlayer = p;
			break;
		}
	}
	if (!targetPlayer) {
		currentEnemyState = EnemyState::Idle;
		updateAnimation();
		return;
	}
	sf::Vector2f toPlayer = targetPlayer->getPosition() - sprite.getPosition();
	
	float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
	float tileSize = 64.0f;
	float distanceTiles = distance / tileSize;

	bool canHit = canBombHitPlayer(targetPlayer->getPosition(), entities);
	// Normal: 60% 
	if (_difficulty == Difficulty::Normal) {
		canHit = canHit && (rand() % 100 < 60);
	}
	if (canHit && bombCooldown.getElapsedTime().asSeconds() > 0.5f) {
		currentEnemyState = EnemyState::PlacingBomb;
		while (!path.empty()) path.pop();
		updateAnimation();
		return;
	}
	if (distanceTiles <= _visionRange) {
		currentEnemyState = EnemyState::Hunting;
	}
	else {
		int roll = rand() % 100;
		if (roll < 50) {
			currentEnemyState = EnemyState::Hunting;
		}
		else {
			currentEnemyState = EnemyState::Looting;
		}
	}
	if (currentEnemyState == EnemyState::Hunting) {
		calculatePathToPlayer(targetPlayer->getPosition(), entities);
		if (path.empty()) {
			
			if (hasAdjacentCrate(entities)
				&& bombCooldown.getElapsedTime().asSeconds() > 0.5f)
			{
				currentEnemyState = EnemyState::PlacingBomb;
			}
			else {
			
				currentEnemyState = EnemyState::Looting;
				
			}
		}
	}
	else if (currentEnemyState == EnemyState::Looting) {
		while (!path.empty()) path.pop();

		sf::Vector2f closestPickupPos;
		float minPickupDist = 999.f;
		bool foundPickup = false;
		for (const auto& ent : entities) {
			if (Pickup* pu = dynamic_cast<Pickup*>(ent.get())) {
				sf::FloatRect bounds = pu->getBounds();
				
				sf::Vector2f puPos(bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f);

				sf::Vector2f diff = puPos - sprite.getPosition();
				float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y) / tileSize;

				if (dist < 7.0f && dist < minPickupDist) {
					minPickupDist = dist;
					closestPickupPos = puPos;
					foundPickup = true;
				}
			}
		}
		if (foundPickup) {
			calculatePathToPlayer(closestPickupPos, entities);
			if (!path.empty()) {
				currentEnemyState = EnemyState::Moving;
				updateAnimation();
				return;
			}
		}
		struct Dir { int x, y; };
		Dir dirs[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
		bool foundCrateToBomb = false;

		
		for (int i = 0; i < 4; ++i) {
			int checkX = static_cast<int>(sprite.getPosition().x / tileSize) + dirs[i].x;
			int checkY = static_cast<int>(sprite.getPosition().y / tileSize) + dirs[i].y;
			sf::FloatRect checkTile({ checkX * tileSize, checkY * tileSize }, { tileSize, tileSize });

			for (const auto& ent : entities) {
				if (ent.get() == this || !ent->isSolid() || dynamic_cast<Player*>(ent.get())) continue;

				
				if (checkTile.findIntersection(ent->getBounds()) && dynamic_cast<Crate*>(ent.get())) {
					foundCrateToBomb = true;
					break;
				}
			}
			if (foundCrateToBomb) break; 
		}

		
		if (foundCrateToBomb && bombCooldown.getElapsedTime().asSeconds() > 0.5f) {
			currentEnemyState = EnemyState::PlacingBomb;
		}
		else {
			
			int randDir = rand() % 4;
			int targetGridX = static_cast<int>(sprite.getPosition().x / tileSize) + dirs[randDir].x;
			int targetGridY = static_cast<int>(sprite.getPosition().y / tileSize) + dirs[randDir].y;
			sf::FloatRect targetTile({ targetGridX * tileSize, targetGridY * tileSize }, { tileSize, tileSize });

			bool hitWall = false;
			for (const auto& ent : entities) {
				if (ent.get() == this || !ent->isSolid() || dynamic_cast<Player*>(ent.get())) continue;
				if (targetTile.findIntersection(ent->getBounds())) {
					hitWall = true;
					break;
				}
			}

			if (!hitWall) {
				path.push({ targetGridX * tileSize, targetGridY * tileSize });
				currentEnemyState = EnemyState::Moving;
			}
			else {
				currentEnemyState = EnemyState::Idle;
			}
		}

	}
	
}
void Enemy::updateAnimation() {
	int col{};
	int row{};
	switch (currentEnemyState) {
	case EnemyState::Idle: col = 0; row = 0; break;
	case EnemyState::Moving: col = 1; row = 0; break;
	case EnemyState::Looting: col = 1; row = 0; break;
	case EnemyState::Hunting: col = 0; row = 1; break;
	case EnemyState::PlacingBomb: col = 0; row = 1; break;
	case EnemyState::PlayerHit: col = 1; row = 1; break;
	case EnemyState::TakeDamage: col = 0; row = 2; break;
	case EnemyState::Dead: col = 1; row = 2; break;
	case EnemyState::Fleeing: col = 0; row = 1; break;
	}
	sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight }));
	};
void Enemy::update(std::vector<std::unique_ptr<Entity>>& entities) {
	if (toBeErased) return;

	
	if (currentEnemyState != EnemyState::Dead
		&& currentEnemyState != EnemyState::TakeDamage)
	{
		for (auto& obj : entities) {
			if (dynamic_cast<ExplosionArea*>(obj.get())) {
				if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
					takeDamage();
					break;
				}
			}
		}
	}
	if (currentEnemyState != EnemyState::Dead
		&& currentEnemyState != EnemyState::TakeDamage
		&& currentEnemyState != EnemyState::PlacingBomb
		&& currentEnemyState != EnemyState::Fleeing)
	{
		int gx = static_cast<int>(std::round(sprite.getPosition().x / 64.f));
		int gy = static_cast<int>(std::round(sprite.getPosition().y / 64.f));

		if (isTileDangerous(gx, gy, entities)) {
			bool shouldFlee = false;

			if (_difficulty == Difficulty::Hard) {
				shouldFlee = true;  
			}
			else {
				// normal diff, 65% chance for fleeing after 0.7s of being in danger
				if (dangerTimer.getElapsedTime().asSeconds() > 0.7f) {
					shouldFlee = (rand() % 100) < 65;
				}
			}

			if (shouldFlee) {
				auto safe = findSafePath(entities);
				if (!safe.empty()) {
					while (!path.empty()) path.pop();
					for (const auto& p : safe) path.push(p);
					currentEnemyState = EnemyState::Fleeing;
					updateAnimation();
				}
			}
		}
		else {
			dangerTimer.restart();  // reset if safe
		}
	}
	float dt = 1.0f / 30.f;
	switch (currentEnemyState)
	{
	case EnemyState::Idle:
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			decideAction(entities);
			actionTimer.restart();
		}
		break;
	case EnemyState::Looting:
		currentEnemyState = EnemyState::Idle;
		break;
	case EnemyState::Hunting:
		
		if (!path.empty()) {
			sf::Vector2f next = path.front();
			int tx = static_cast<int>(std::round(next.x / 64.f));
			int ty = static_cast<int>(std::round(next.y / 64.f));
			if (isTileSolid(tx, ty, entities)) {
				while (!path.empty()) path.pop();
				decideAction(entities);
				actionTimer.restart();
				break;
			}
		}
		followPath(dt);
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			decideAction(entities);
			actionTimer.restart();
		}
		break;

	case EnemyState::Moving:
		if (!path.empty()) {
			sf::Vector2f next = path.front();
			int tx = static_cast<int>(std::round(next.x / 64.f));
			int ty = static_cast<int>(std::round(next.y / 64.f));
			if (isTileSolid(tx, ty, entities)) {
				while (!path.empty()) path.pop();
				decideAction(entities);
				actionTimer.restart();
				break;
			}
		}
		followPath(dt);
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			decideAction(entities);
			actionTimer.restart();
		}
		break;
	case EnemyState::PlacingBomb:
		if (bombCooldown.getElapsedTime().asSeconds() > 0.40f) {
			int myActiveBombs = 0;
			for (const auto& obj : entities) {
				if (Bomb* b = dynamic_cast<Bomb*>(obj.get()))
					if (b->getOwner() == this) myActiveBombs++;
			}

			if (myActiveBombs < _bombAmount) {
				int gridX = static_cast<int>(std::round(sprite.getPosition().x / 64.f));
				int gridY = static_cast<int>(std::round(sprite.getPosition().y / 64.f));

				
				auto escape = findEscapePath(gridX, gridY, _currentBombStats, entities);

				if (escape.empty()) {
					
					currentEnemyState = EnemyState::Idle;
					updateAnimation();
					break;
				}

				entities.push_back(std::make_unique<Bomb>(
					gridX * 64.f, gridY * 64.f,
					_bombTex, _expTex, _currentBombStats, bombSoundBuf, this));
				bombCooldown.restart();

				while (!path.empty()) path.pop();
				for (const auto& p : escape) path.push(p);

				currentEnemyState = EnemyState::Fleeing; 
				actionTimer.restart();
			}
			else {
				currentEnemyState = EnemyState::Idle;
			}
			updateAnimation();
		}
		break;

	case EnemyState::Fleeing:
		if (!path.empty()) {
			sf::Vector2f next = path.front();
			int tx = static_cast<int>(std::round(next.x / 64.f));
			int ty = static_cast<int>(std::round(next.y / 64.f));
			if (isTileSolid(tx, ty, entities)) {
			
				auto safe = findSafePath(entities);
				while (!path.empty()) path.pop();
				if (!safe.empty())
					for (const auto& p : safe) path.push(p);
				else
					currentEnemyState = EnemyState::Idle;
				updateAnimation();
				break;
			}
		}
		followPath(1.0f / 30.f);
		if (path.empty()) {
			currentEnemyState = EnemyState::Idle;
			updateAnimation();
		}
		break;
	case EnemyState::PlayerHit:
		//tbi
		break;
	case EnemyState::TakeDamage:
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			currentEnemyState = EnemyState::Idle;
			updateAnimation();
		}
		break;
	case EnemyState::Dead:
		toBeErased = true;
		break;
	default:
		break;
	}
}
void Enemy::followPath(float deltaTime) {
	if (path.empty()) {
		currentEnemyState = EnemyState::Idle;
		updateAnimation();
		return;
	}
	sf::Vector2f target = path.front();
	sf::Vector2f currentPos = sprite.getPosition();
	sf::Vector2f direction = target - currentPos;
	float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	float movement = _speed;	
	if (distance < movement	) {
		sprite.setPosition(target);
		path.pop();
		if (path.empty()) {
			actionTimer.restart();
	}
	}
	else {
		direction.x /= distance;
		direction.y /= distance;
		sprite.move(direction * _speed);
	}
}
bool Enemy::isTileSolid(int gridX, int gridY, const std::vector<std::unique_ptr<Entity>>& entities) const {
	float tileSize = 64.0f;
	sf::FloatRect tileRect({ gridX * tileSize, gridY * tileSize }, { tileSize, tileSize });
	for (const auto& obj : entities) {
		if (obj.get() == this) continue;
		if (dynamic_cast<Player*>(obj.get())) continue;
		if (ExplosionArea* fire = dynamic_cast<ExplosionArea*>(obj.get())) {
			
			if (fire->isVisible() && tileRect.findIntersection(fire->getBounds())) {
				return true;
			}
			continue; 
		}
		if (dynamic_cast<Bomb*>(obj.get())) {
			if (tileRect.findIntersection(obj->getBounds())) {
				return true;
			}
			continue;
		}
		if (!obj->isSolid()) continue;

		if (tileRect.findIntersection(obj->getBounds())) {
			return true;
		}
	}

	return false;
}
void Enemy::calculatePathToPlayer(const sf::Vector2f& playerPos,
	const std::vector<std::unique_ptr<Entity>>& entities) {
	while (!path.empty()) path.pop();
	constexpr float tileSize = 64.0f;
	GridPoint start{ static_cast<int>(std::round(sprite.getPosition().x / tileSize)),
					 static_cast<int>(std::round(sprite.getPosition().y / tileSize)) };
	GridPoint goal{ static_cast<int>(std::round(playerPos.x / tileSize)),
					 static_cast<int>(std::round(playerPos.y / tileSize)) };

	auto runBFS = [&](bool avoidDanger) -> std::vector<sf::Vector2f> {
		std::queue<GridPoint> frontier;
		std::map<GridPoint, GridPoint> cameFrom;
		frontier.push(start);
		cameFrom[start] = start;
		GridPoint dirs[4] = { {0,-1},{0,1},{-1,0},{1,0} };
		bool found = false;

		while (!frontier.empty()) {
			GridPoint curr = frontier.front();
			frontier.pop();
			if (curr == goal) { found = true; break; }
			for (auto& d : dirs) {
				GridPoint next{ curr.x + d.x, curr.y + d.y };
				if (cameFrom.count(next)) continue;
				if (isTileSolid(next.x, next.y, entities)) continue;
				if (avoidDanger && !(next == goal) && isTileDangerous(next.x, next.y, entities))
					continue;
				frontier.push(next);
				cameFrom[next] = curr;
			}
		}
		std::vector<sf::Vector2f> result;
		if (!found) return result;
		GridPoint c = goal;
		while (!(c == start)) {
			result.push_back({ c.x * tileSize, c.y * tileSize });
			c = cameFrom[c];
		}
		std::reverse(result.begin(), result.end());
		return result;
		};

	auto result = runBFS(true);           // check safety
	if (result.empty()) result = runBFS(false);  // then risk
	for (const auto& p : result) path.push(p);
}
void Enemy::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}
void Enemy::takeDamage() {
	_health--;
	if (_health <= 0) {
		currentEnemyState = EnemyState::Dead;
		toBeErased = true;
	}
	else {
		currentEnemyState = EnemyState::TakeDamage;
		actionTimer.restart();
	}
	updateAnimation();
}
void Enemy::addBombRange(int left, int right, int up, int down) {
	_currentBombStats.rangeLeft += left;
	if (_currentBombStats.rangeLeft < 1) {
		_currentBombStats.rangeLeft = 1;
	}
	_currentBombStats.rangeRight += right;
	if (_currentBombStats.rangeRight < 1) {
		_currentBombStats.rangeRight = 1;
	}
	_currentBombStats.rangeUp += up;
	if (_currentBombStats.rangeUp < 1) {
		_currentBombStats.rangeUp = 1;
	}
	_currentBombStats.rangeDown += down;
	if (_currentBombStats.rangeDown < 1) {
		_currentBombStats.rangeDown = 1;
	}
}
void Enemy::speedUp(float newVal) {
	_speed += newVal;
	if (_speed < 5.5f) {
		_speed = 5.5f;
	}
	if (_speed >= 12.5f) {
		_speed = 12.5f;
	}
}
void Enemy::addBombAmount(int newVal) {
	_bombAmount += newVal;
	if (_bombAmount < 1) {
		_bombAmount = 1;
	}
}
void Enemy::hpUp(int newVal) {
	_health += newVal;
	if (_health < 1) {
		_health = 1;
	}
}
void Enemy::setHp(int newVal) {
	_health = newVal;
	if (_health < 1) _health = 1;
}
bool Enemy::isTileDangerous(int gridX, int gridY,
	const std::vector<std::unique_ptr<Entity>>& entities) const {
	const float tileSize = 64.0f;

	for (const auto& obj : entities) {
		const Bomb* bomb = dynamic_cast<const Bomb*>(obj.get());
		if (!bomb) continue;

		sf::FloatRect bb = bomb->getBounds();
		int bx = static_cast<int>(std::round((bb.position.x + bb.size.x / 2.f) / tileSize));
		int by = static_cast<int>(std::round((bb.position.y + bb.size.y / 2.f) / tileSize));
		const BombStats& s = bomb->getStats();

		if (gridX == bx && gridY == by) return true;

		if (gridX == bx) {
			int dy = gridY - by;
			if (dy < 0 && -dy <= s.rangeUp)    return true;
			if (dy > 0 && dy <= s.rangeDown)  return true;
		}
		if (gridY == by) {
			int dx = gridX - bx;
			if (dx < 0 && -dx <= s.rangeLeft)  return true;
			if (dx > 0 && dx <= s.rangeRight) return true;
		}
	}
	return false;
}

std::vector<sf::Vector2f> Enemy::findEscapePath(
	int bombX, int bombY,
	const BombStats& stats,
	const std::vector<std::unique_ptr<Entity>>& entities) const
{
	constexpr float tileSize = 64.0f;
	GridPoint start{
		static_cast<int>(std::round(sprite.getPosition().x / tileSize)),
		static_cast<int>(std::round(sprite.getPosition().y / tileSize))
	};

	auto inNewBlast = [&](GridPoint p) {
		if (p.x == bombX && p.y == bombY) return true;
		if (p.x == bombX) {
			int dy = p.y - bombY;
			if (dy < 0 && -dy <= stats.rangeUp)   return true;
			if (dy > 0 && dy <= stats.rangeDown) return true;
		}
		if (p.y == bombY) {
			int dx = p.x - bombX;
			if (dx < 0 && -dx <= stats.rangeLeft)  return true;
			if (dx > 0 && dx <= stats.rangeRight) return true;
		}
		return false;
		};

	std::queue<GridPoint> frontier;
	std::map<GridPoint, GridPoint> cameFrom;
	std::map<GridPoint, int> depth;
	frontier.push(start);
	cameFrom[start] = start;
	depth[start] = 0;

	GridPoint dirs[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
	constexpr int MAX_DEPTH = 25;

	while (!frontier.empty()) {
		GridPoint curr = frontier.front();
		frontier.pop();

		if (!inNewBlast(curr) && !isTileDangerous(curr.x, curr.y, entities)) {
			std::vector<sf::Vector2f> path;
			GridPoint c = curr;
			while (!(c == start)) {
				path.push_back({ c.x * tileSize, c.y * tileSize });
				c = cameFrom[c];
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		if (depth[curr] >= MAX_DEPTH) continue;

		for (auto& d : dirs) {
			GridPoint next{ curr.x + d.x, curr.y + d.y };
			if (cameFrom.count(next)) continue;
			if (isTileSolid(next.x, next.y, entities)) continue;
			frontier.push(next);
			cameFrom[next] = curr;
			depth[next] = depth[curr] + 1;
		}
	}
	return {};
}

std::vector<sf::Vector2f> Enemy::findSafePath(
	const std::vector<std::unique_ptr<Entity>>& entities) const
{
	constexpr float tileSize = 64.0f;
	GridPoint start{
		static_cast<int>(std::round(sprite.getPosition().x / tileSize)),
		static_cast<int>(std::round(sprite.getPosition().y / tileSize))
	};

	std::queue<GridPoint> frontier;
	std::map<GridPoint, GridPoint> cameFrom;
	frontier.push(start);
	cameFrom[start] = start;
	GridPoint dirs[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	while (!frontier.empty()) {
		GridPoint curr = frontier.front();
		frontier.pop();
		if (!isTileDangerous(curr.x, curr.y, entities)) {
			std::vector<sf::Vector2f> path;
			GridPoint c = curr;
			while (!(c == start)) {
				path.push_back({ c.x * tileSize, c.y * tileSize });
				c = cameFrom[c];
			}
			std::reverse(path.begin(), path.end());
			return path;
		}
		for (auto& d : dirs) {
			GridPoint next{ curr.x + d.x, curr.y + d.y };
			if (cameFrom.count(next)) continue;
			if (isTileSolid(next.x, next.y, entities)) continue;
			frontier.push(next);
			cameFrom[next] = curr;
		}
	}
	return {};
}
bool Enemy::canBombHitPlayer(const sf::Vector2f& playerPos,
	const std::vector<std::unique_ptr<Entity>>& entities) const {
	const float tileSize = 64.0f;
	int ex = static_cast<int>(std::round(sprite.getPosition().x / tileSize));
	int ey = static_cast<int>(std::round(sprite.getPosition().y / tileSize));
	int px = static_cast<int>(std::round(playerPos.x / tileSize));
	int py = static_cast<int>(std::round(playerPos.y / tileSize));

	if (ex != px && ey != py) return false;  

	auto wallAt = [&](int gx, int gy) {
		sf::FloatRect tile({ gx * tileSize, gy * tileSize }, { tileSize, tileSize });
		for (const auto& obj : entities) {
			if (!dynamic_cast<Wall*>(obj.get()) && !dynamic_cast<Crate*>(obj.get())) continue;
			if (tile.findIntersection(obj->getBounds())) return true;
		}
		return false;
		};

	if (ex == px) {
		int dy = py - ey;
		int range = (dy < 0) ? _currentBombStats.rangeUp : _currentBombStats.rangeDown;
		if (std::abs(dy) > range || std::abs(dy) == 0) return false;
		int step = (dy > 0) ? 1 : -1;
		for (int y = ey + step; y != py; y += step)
			if (wallAt(ex, y)) return false;
		return true;
	}
	int dx = px - ex;
	int range = (dx < 0) ? _currentBombStats.rangeLeft : _currentBombStats.rangeRight;
	if (std::abs(dx) > range || std::abs(dx) == 0) return false;
	int step = (dx > 0) ? 1 : -1;
	for (int x = ex + step; x != px; x += step)
		if (wallAt(x, ey)) return false;
	return true;
}
bool Enemy::hasAdjacentCrate(const std::vector<std::unique_ptr<Entity>>& entities) const {
	constexpr float tileSize = 64.0f;
	int ex = static_cast<int>(std::round(sprite.getPosition().x / tileSize));
	int ey = static_cast<int>(std::round(sprite.getPosition().y / tileSize));
	int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
	for (auto& d : dirs) {
		sf::FloatRect tile(
			{ (ex + d[0]) * tileSize, (ey + d[1]) * tileSize },
			{ tileSize, tileSize });
		for (const auto& obj : entities) {
			if (dynamic_cast<Crate*>(obj.get())
				&& tile.findIntersection(obj->getBounds()))
				return true;
		}
	}
	return false;
}
void Enemy::setDifficulty(Difficulty d) {
	_difficulty = d;
	if (d == Difficulty::Normal) {
		_speed = 3.5f;
		actionDurr = 1.65f;   // takes more time to rhink and place bomb, giving player more time to react
		_visionRange = 6.0f;
		_currentBombStats = {1, 1, 1, 1};  // players range 
	}
	else {
		_speed = 4.5f;
		actionDurr = 0.65f;
		_visionRange = 10.0f;
		_currentBombStats = { 2, 2, 2, 2 };
	}
}
void Enemy::takeCurseDamage() {
	//grace period for curse mechanic to prevent instant death from multiple sources at once, as curse damage is unavoidable and can be applied by multiple sources in the same frame
	if (_curseHitTimer.getElapsedTime().asSeconds() < 1.0f) return;
	_curseHitTimer.restart();
	takeDamage();
}