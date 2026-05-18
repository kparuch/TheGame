#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

class Crate :public Entity {
private:
	sf::Sprite sprite;
	bool destroyed = false;
public:
	Crate(float x, float y, const sf::Texture& texture);
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override;
	bool isSolid() const override { return true; }
	void draw(sf::RenderWindow& window) override;
	bool isDestroyed()const override { return destroyed; }
	void destroy()override { destroyed = true; }
};