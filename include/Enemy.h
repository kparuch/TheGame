#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <SFML/System/Clock.hpp>
#include "Bomb.h"
enum class EnemyState {
	Idle, //doesnt move 
	Moving, //moves
	Looting, //breaks crates, collects powerups
	Hunting, //targets player, initiating the attack
	PlacingBomb, //placing bomb
	PlayerHit, //bomb damages the player 
	TakeDamage, //entity detects hit
	Dead,//is dead
	Fleeing 
};
enum class Difficulty { Normal, Hard };
class Enemy :public Entity {
private:
	sf::Sprite sprite;
	EnemyState currentEnemyState;
	sf::Clock actionTimer; //to manage state durations
	sf::Clock animiationDurationTimer; //to manage animation frames
	std::queue<sf::Vector2f> path; //for pathfinding
	int _health = 3; //same paratmeters as player, might be changed later depending on difficulty settings (TBI)
	BombStats _currentBombStats;
	float _speed{0.0f};
	int _bombAmount=1;
	float actionDurr = 1.1f; //action duration for proper sprite animation
	sf::Vector2f _direction; //current movement direction
	bool toBeErased=false;
	bool canKickBombs = false;
	bool laserBombs = false;
	sf::Clock bombCooldown; //to prevent clumping of bomb placements
	float _visionRange=5.0f; //when player is within this range, enemy will start hunting
	const sf::Texture& _bombTex; // reference to bomb texture for bomb placement
	const sf::Texture& _expTex; // reference to explosion texture of a bomb
	void decideAction(const std::vector<std::unique_ptr<Entity>>&entities);
	void calculatePathToPlayer(const sf::Vector2f& playerPos, const std::vector<std::unique_ptr<Entity>>& entities);
	void followPath(float deltaTime);
	bool isTileSolid(int gridX, int gridY, const std::vector<std::unique_ptr<Entity>>& entities)const;
	void updateAnimation();
	int frameWidth;
	int frameHeight;
	const sf::SoundBuffer& bombSoundBuf;
	bool isTileDangerous(int gridX, int gridY, const std::vector<std::unique_ptr<Entity>>& entities) const;
	bool canBombHitPlayer(const sf::Vector2f& playerPos, const std::vector<std::unique_ptr<Entity>>& entities) const;
	std::vector<sf::Vector2f> findEscapePath(int bombX, int bombY, const BombStats& stats,
		const std::vector<std::unique_ptr<Entity>>& entities) const;
	std::vector<sf::Vector2f> findSafePath(const std::vector<std::unique_ptr<Entity>>& entities) const;
	int _maxHealth = 1;
	Difficulty _difficulty = Difficulty::Normal;
	sf::Clock dangerTimer;
	sf::Clock _curseHitTimer;
	int ogHealth = 3;
public:
	Enemy(float x, float y, const sf::Texture& texture, const sf::Texture& bombTex, const sf::Texture& expTex, float visionRange, const sf::SoundBuffer &bombSound);
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;
	void takeDamage();
	bool isSolid() const override { return true; }
	bool isDestroyed() const override { return toBeErased; }
	void addBombRange(int left, int right, int up, int down);
	void speedUp(float newVal);
	void addBombAmount(int newVal);
	void hpUp(int newVal);
	void setHp(int newVal);
	bool hasAdjacentCrate(const std::vector<std::unique_ptr<Entity>>& entities) const;
	void setDifficulty(Difficulty d);
	void takeCurseDamage();
	int getHealth() const { return _health; }

};
