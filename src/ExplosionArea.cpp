#include "ExplosionArea.h"
ExplosionArea::ExplosionArea(float x, float y, const sf::Texture& texture)
    : sprite(texture), toBeErased(false)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.25f, 0.25f });
}
void ExplosionArea::update(std::vector<std::unique_ptr<Entity>>&entities) {
    if (czas.getElapsedTime().asSeconds() >= 1.0f) {
        toBeErased = true;
    }
}

void ExplosionArea::draw(sf::RenderWindow& window) {
    window.draw(sprite);
};