#include "Wall.h"
Wall::Wall(float x, float y, const sf::Texture& texture):sprite(texture){
	sprite.setPosition({ x,y });
	sprite.setScale({ 0.25f,0.25f });
};
void Wall::update(std::vector<std::unique_ptr<Entity>>& entities) {

}

sf::FloatRect Wall::getBounds() const {
	return sprite.getGlobalBounds(); 
}

void Wall::draw(sf::RenderWindow& window) {
	window.draw(sprite);
}
void Wall::setCrused(const sf::Texture& text, sf::IntRect frame) {
	sprite.setTexture(text);
	sprite.setTextureRect(frame);
	sprite.setScale({ 0.25f,0.25f });
}