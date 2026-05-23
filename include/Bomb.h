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
struct BombStats {
	int rangeLeft = 1;
	int rangeRight = 1;
	int rangeUp = 1;
	int rangeDown = 1;
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
	bool forceExplode = false;
	bool isPassable;
	BombStats _stats;
	const sf::SoundBuffer &explosionSound;
public:
	Bomb(float x, float y, const sf::Texture& bombText, const sf::Texture &expText ,BombStats _stats, const sf::SoundBuffer & explosionSound);
	~Bomb() = default;
	bool isSolid() const override{ return !isPassable; }
	bool isDestroyed() const override { return isExploded;}
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds(); }
	void draw(sf::RenderWindow& window) override;
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	void triggerExplosion() { forceExplode = true; }
};
