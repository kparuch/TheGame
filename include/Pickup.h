#pragma once
#include "Entity.h"
//#include "Enemy.h
#include <SFML/System/Clock.hpp>

class Player;

class Pickup : public Entity {
protected:
	sf::Sprite sprite;
	bool isPickedUp;
	sf::Clock animTimer;
	int frameWidth;
	int frameHeight;
	void updateAnimation();
	sf::Clock graceTimer;
	float graceDur = 0.6f;
public:
	Pickup(float x, float y, const sf::Texture& texture);
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	void draw(sf::RenderWindow& window)override;
	sf::FloatRect getBounds()const override { return sprite.getGlobalBounds(); }
	bool isSolid() { return false;}
	bool isDestroyed() const override	{ return isPickedUp; }
	virtual void applyEffect(Player* p) = 0;
	void burn();
};