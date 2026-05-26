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

	if (distanceTiles <= 1.5f && bombCooldown.getElapsedTime().asSeconds() > 0.5f) {
		currentEnemyState = EnemyState::PlacingBomb;
		while (!path.empty())path.pop();
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
			currentEnemyState = EnemyState::Idle;
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

		
		if (foundCrateToBomb && bombCooldown.getElapsedTime().asSeconds() > 3.0f) {
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
	}
	sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight }));
	};
void Enemy::update(std::vector<std::unique_ptr<Entity>>& entities) {
	if (toBeErased) return;
	if (currentEnemyState != EnemyState::Dead && currentEnemyState != EnemyState::TakeDamage) {
		for (auto& obj : entities) {
			if (dynamic_cast<ExplosionArea*>(obj.get())) {
				if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
					takeDamage(); 
					break;
				}
			}
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
	case EnemyState::Moving:
		followPath(dt);
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			updateAnimation();
			actionTimer.restart();
		}
		break;
		break;
	case EnemyState::Looting:
		break;
	case EnemyState::Hunting:
		followPath(dt);
		if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
			updateAnimation();
			actionTimer.restart();
		}
		break;
	case EnemyState::PlacingBomb:
		if (bombCooldown.getElapsedTime().asSeconds() > 0.40f)
		{
			int myActiveBombs = 0;
			for (const auto& obj : entities) {
				if (Bomb* b = dynamic_cast<Bomb*>(obj.get())) {
					if (b->getOwner() == this) {
						myActiveBombs++;
					}
				}
			}

			if (myActiveBombs < _bombAmount) {
				float gridX = std::round(sprite.getPosition().x / 64.0f) * 64.0f;
				float gridY = std::round(sprite.getPosition().y / 64.0f) * 64.0f;

				entities.push_back(std::make_unique<Bomb>(gridX, gridY, _bombTex, _expTex, _currentBombStats, bombSoundBuf, this));
				bombCooldown.restart();

				
				GridPoint start = { static_cast<int>(gridX / 64.0f), static_cast<int>(gridY / 64.0f) };
				std::queue<GridPoint> frontier;
				std::map<GridPoint, GridPoint> cameFrom;
				frontier.push(start);
				cameFrom[start] = start;

				GridPoint dirs[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
				GridPoint safeGoal = { -1, -1 };

				
				while (!frontier.empty()) {
					GridPoint curr = frontier.front();
					frontier.pop();

					
					bool cornerSafe = (curr.x != start.x && curr.y != start.y);
					bool distanceSafe = (std::abs(curr.x - start.x) > 3 || std::abs(curr.y - start.y) > 3);

					if (cornerSafe || distanceSafe) {
						safeGoal = curr;
						break; 
					}

					for (auto& dir : dirs) {
						GridPoint next = { curr.x + dir.x, curr.y + dir.y };

						
						if (cameFrom.find(next) == cameFrom.end() && !isTileSolid(next.x, next.y, entities)) {
							frontier.push(next);
							cameFrom[next] = curr;
						}
					}
				}

				
				if (safeGoal.x != -1) {
					std::vector<sf::Vector2f> escapePath;
					GridPoint curr = safeGoal;
					while (!(curr == start)) {
						escapePath.push_back({ curr.x * 64.0f, curr.y * 64.0f });
						curr = cameFrom[curr];
					}
					std::reverse(escapePath.begin(), escapePath.end());

					for (const auto& p : escapePath) {
						path.push(p);
					}
					currentEnemyState = EnemyState::Moving; 
					actionTimer.restart();
				}
				else {
					currentEnemyState = EnemyState::Idle; 
				}
			}
		}

		if (currentEnemyState != EnemyState::Moving) {
			currentEnemyState = EnemyState::Idle;
		}

		updateAnimation();
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
		if (!obj->isSolid()) continue;
		if (dynamic_cast<Player*>(obj.get())) continue;
		if (tileRect.findIntersection(obj->getBounds())) {
			return true;
		}
	}
	return false;
}
void Enemy::calculatePathToPlayer(const sf::Vector2f& playerPos, const std::vector<std::unique_ptr<Entity>>& entities) {
	while (!path.empty()) path.pop();
	float tileSize = 64.0f;

	GridPoint start{ static_cast<int>(sprite.getPosition().x / tileSize), static_cast<int>(sprite.getPosition().y / tileSize) };
	GridPoint goal{ static_cast<int>(playerPos.x / tileSize), static_cast<int>(playerPos.y / tileSize) };
	std::queue<GridPoint> frontier;
	std::map<GridPoint, GridPoint> cameFrom;
	frontier.push(start);
	cameFrom[start] = start;
	GridPoint dirs[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} }; //left, right , down, up

	bool found = false;

	while (!frontier.empty()) {
		GridPoint current = frontier.front();
		frontier.pop();
		if (current == goal) {
			found = true;
			break;
		}
		for (auto& dir : dirs) {
			GridPoint next{ current.x + dir.x, current.y + dir.y };
			if (cameFrom.find(next) == cameFrom.end() && !isTileSolid(next.x, next.y, entities)) {
				frontier.push(next);
				cameFrom[next] = current;
			}
		}
	}
	if (found) {
		std::vector<sf::Vector2f> reversePath;	
		GridPoint current = goal;

		while (!(current == start))
		{
			reversePath.push_back({ current.x * tileSize, current.y * tileSize });
			current = cameFrom[current];
		}
		std::reverse(reversePath.begin(), reversePath.end());
		for (const auto& p : reversePath) {
			path.push(p);
		}
	}




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
	if (_health > 1) {
		_health = 1;
	}
}
void Enemy::setHp(int newVal) {
	_health = newVal;
	if (_health < 1) {
		_health = 1;
	}
}