#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
class Wall :public Entity {
private:
	sf::Sprite sprite;
public:
	Wall(float x, float y, const sf::Texture& texture);
	void update(std::vector<std::unique_ptr<Entity>>& entities) override;
	sf::FloatRect getBounds() const override;
	bool isSolid() const override { return true; }
	void draw(sf::RenderWindow& window) override;
	void setCrused(const sf::Texture& curseText, sf::IntRect frame);
};