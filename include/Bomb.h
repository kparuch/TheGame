#pragma once
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <SFML/System/Clock.hpp>
#include "Entity.h"
enum class BombState {
	Placed,
	LightUp,
	Exploded
};
class Bomb : public Entity {
private:
	sf::Sprite sprite;
	sf::Clock timer;
	bool isExploded;
	int _range;
	const sf::Texture& explosionTexture;
	void updateAnimation();
	BombState currentBombState;
	int frameWidth;
	int frameHeight;
public:
	Bomb(float x, float y, const sf::Texture& bombText, const sf::Texture &expText);
	~Bomb() = default;
	bool isSolid()const override{ return false; }
	bool isDestroyed() const override { return isExploded;}
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
};
