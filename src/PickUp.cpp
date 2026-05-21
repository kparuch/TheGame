#include "Pickup.h"
#include "Player.h"

Pickup::Pickup(float x, float y, const sf::Texture& texture)
    : isPickedUp(false), sprite(texture)
{
    sprite.setTexture(texture);

   
    frameWidth = texture.getSize().x / 2;
    frameHeight = texture.getSize().y / 2;
    sprite.setTextureRect(sf::IntRect({ 0, 0 }, { frameWidth, frameHeight }));
    sprite.setScale({ 0.225f, 0.225f });
    sprite.setPosition({ x, y });
}
void Pickup::updateAnimation() {
    float time = animTimer.getElapsedTime().asSeconds();
    int currentFrame = static_cast<int>(time / 0.2f) % 4;
    int col = currentFrame % 2;
    int row = currentFrame / 2;
    sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth,frameHeight }));
}
void Pickup::update(std::vector<std::unique_ptr<Entity>>& entites) {
    if (isPickedUp)return;
    updateAnimation();

    for (auto& obj : entites) {
        if (Player* p = dynamic_cast<Player*>(obj.get())) {
            if (sprite.getGlobalBounds().findIntersection(p->getBounds())) {
                applyEffect(p);
                isPickedUp = true;
                break;
            }
        }
    }
}
void Pickup::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
void Pickup::burn() {
    if (graceTimer.getElapsedTime().asSeconds() > graceDur) {
        isPickedUp = true;
    }
}
void::Pickup::destroy() {
    isPickedUp = true;
}