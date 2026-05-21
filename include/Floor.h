#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

class Floor :public Entity {
private:
	sf::Sprite sprite;
public:
	Floor(float x, float y, const sf::Texture& texture);
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override;
	void draw(sf::RenderWindow& window) override;
	void setCursed(const sf::Texture& tex) {
		sprite.setTexture(tex,true);
	}
};