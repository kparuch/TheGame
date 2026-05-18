#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

class Crate :public Entity {
private:
	sf::Sprite sprite;
public:
	Crate(float x, float y, const sf::Texture& texture);
	void update(const std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override;
	bool isSolid() const override { return true; }
	void draw(sf::RenderWindow& window) override;
};