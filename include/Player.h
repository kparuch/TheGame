#pragma once
#include "Entity.h"
#include <SFML/System/Clock.hpp>
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
	float _speed=5.5f;
	int _bombAmount=3;
	int _bombRange=2;
	int _hp=1;
	PlayerState currentState;
	int frameWidth;
	int frameHeight;
	void updateAnimation();
	const sf::Texture& bombTexRef;
	const sf::Texture& explTexRef;
	sf::Clock bombCooldown;
	sf::Clock actionTimer;
	float actionDurr = 1.1f;

public:
	Player(float x, float y, const sf::Texture& texture, const sf::Texture& bombTexRef,const sf::Texture &explTexRef);

	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;
	void takeDamage();
	bool isDead() { return currentState == PlayerState::Dead; }
	void addHp(int amount);
	void addBomb(int amount);
	void speedUp(float val);
	void addRange(int amount);
};