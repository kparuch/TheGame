#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "Entity.h"
#include <SFML/Audio.hpp>

class ExplosionArea :public Entity {
private:
	sf::Sprite sprite;
	sf::Clock czas;
	bool toBeErased;
	sf::Sound sound;
	bool _isVisible;
	
public:
	ExplosionArea(float x, float y, const sf::Texture& texture, const sf::SoundBuffer &soundBuffer, bool playSound);
	void update(std::vector<std::unique_ptr<Entity>>& entities)override;
	void draw(sf::RenderWindow &window)override;
	sf::FloatRect getBounds() const override { return sprite.getGlobalBounds();}
	bool isSolid()const override { return false; }
	bool isDestroyed() const override { return toBeErased; }
	bool isVisible()  { return _isVisible; }
}; 
