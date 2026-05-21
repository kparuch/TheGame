#pragma once
#include "Entity.h"
#include <SFML/System/Clock.hpp>
#include "Bomb.h"
enum class CursedState {
	Idle,
	Move,
	Hit,
	PostHit
};
enum class PlayerState {
	Idle,
	Moving,
	PlacingBomb,
	HitEnemy,
	TakeDamage,
	Dead
};
class Player : public Entity {
private:
	sf::Sprite sprite;
	float _speed=3.5f;
	int _bombAmount=1;
	BombStats _currentBombStats;
	int _hp=3;
	PlayerState currentState;
	int frameWidth;
	int frameHeight;
	void updateAnimation();
	const sf::Texture& bombTexRef;
	const sf::Texture& explTexRef;
	sf::Clock bombCooldown;
	sf::Clock actionTimer;
	float actionDurr = 1.1f;
	bool _isCursed = false;
	bool _hasDebuff = false;
	sf::Clock curseTime;
	sf::Clock debuffTime;
	sf::Clock idleTimer;
	float _ogSpeed = 5.5f;
	sf::Vector2f lastPos;
	sf::Texture curseTex;
	CursedState curseState;
	sf::Texture normTex;
	sf::Clock hitAnimTimer;


public:
	Player(float x, float y, const sf::Texture& texture, const sf::Texture& bombTexRef,const sf::Texture &explTexRef, const sf::Texture &curseTex);

	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;
	void takeDamage(bool byPassGrace = false);
	bool isDead() { return currentState == PlayerState::Dead; }
	void addHp(int amount);
	void addBomb(int amount);
	void speedUp(float val);
	
	void activateCurse();
	bool isCursed()const { return _isCursed; }
	void resetIdleTimer() { idleTimer.restart(); }
	void setHp(int val);
	void setAnimState(CursedState state);
	void addBombRange(int newLeft, int newRight, int newUp, int newDwn);
};