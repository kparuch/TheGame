#include "Floor.h"
Floor::Floor(float x, float y, const sf::Texture& texture) :sprite(texture) {
	sprite.setPosition({ x,y });
	sprite.setScale({ 0.25f,0.25f });
};
void Floor::update(const std::vector<std::unique_ptr<Entity>>& entities) {};


sf::FloatRect Floor::getBounds() const {
	return sprite.getGlobalBounds(); // SFML sam wyliczy prostokąt naszego sprite'a
}
void Floor::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}