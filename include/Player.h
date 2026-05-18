#pragma once
#include "Entity.h"
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
	float _speed;
	float _bombAmount;
	float _bombRange;
	int _hp;
	PlayerState currentState;
	int frameWidth;
	int frameHeight;
	void updateAnimation();
public:
	Player(float x, float y, const sf::Texture& texture);

	void update(const std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;

};