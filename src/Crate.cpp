#include "Crate.h"
Crate::Crate(float x, float y, const sf::Texture& texture) :sprite(texture) {
	sprite.setPosition({ x,y });
	sprite.setScale({ 0.25f,0.25f });
};
void Crate::update( std::vector<std::unique_ptr<Entity>>& entities) {
	
}

sf::FloatRect Crate::getBounds() const {
	return sprite.getGlobalBounds(); // SFML sam wyliczy prostokąt naszego sprite'a
}

void Crate::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}
