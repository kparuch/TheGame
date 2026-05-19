#include "ExplosionArea.h"
#include "Pickup.h"
ExplosionArea::ExplosionArea(float x, float y, const sf::Texture& texture)
    : sprite(texture), toBeErased(false)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.25f, 0.25f });
}
void ExplosionArea::update(std::vector<std::unique_ptr<Entity>>&entities) {
    if (czas.getElapsedTime().asSeconds() >= 0.5f) {
        toBeErased = true;
        return;
    }
    for (auto& obj : entities) {
        if (Pickup* pickup = dynamic_cast<Pickup*>(obj.get())) {
            if (sprite.getGlobalBounds().findIntersection(pickup->getBounds())) {
                pickup->burn();
            }
        }
     }
}

void ExplosionArea::draw(sf::RenderWindow& window) {
    window.draw(sprite);
};