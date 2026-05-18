#pragma once
#include <SFML/Graphics.hpp>
class Entity {
public:
	virtual ~Entity() = default;
	virtual void update(const std::vector<std::unique_ptr<Entity>>& entities) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::FloatRect getBounds() const = 0;
	virtual bool isSolid() const { return false; }
};