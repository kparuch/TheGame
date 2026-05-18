#include "Player.h"
Player::Player(float x, float y, const sf::Texture& texture)
    : sprite(texture), _speed(10.0f), currentState(PlayerState::Idle)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.20f, 0.20f });
    frameWidth = texture.getSize().x / 2;
    frameHeight = texture.getSize().y / 3;

    
    updateAnimation();
}
void Player::updateAnimation() {
    int col{};
    int row{};
    switch (currentState) {
    case PlayerState::Idle: col = 0; row = 0; break;
    case PlayerState::Moving: col = 1; row = 0; break;
    case PlayerState::PlacingBomb: col = 0; row = 1; break;
    case PlayerState::HitEnemy: col = 1; row = 1; break;
    case PlayerState::TakeDamage: col = 0; row = 2; break;
    case PlayerState::Dead: col = 1; row = 2; break;
    }
    sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight }));
}
void Player::update(const std::vector<std::unique_ptr<Entity>>& entities) {
    sf::Vector2f movement({ 0.f, 0.f });
    bool isMoving = false;

    // Sprawdzamy klawisze
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { movement.y -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { movement.y += _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { movement.x -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { movement.x += _speed; isMoving = true; }
 
    if (currentState != PlayerState::Dead && currentState != PlayerState::TakeDamage) {
        if (isMoving) currentState = PlayerState::Moving;
        else currentState = PlayerState::Idle;
    }

    // --- OS X ---
    sprite.move({ movement.x, 0.f }); // Przesuwamy tylko w poziomie
    for (auto& obj : entities) {
        // Nie kolidujemy z samym sobą i sprawdzamy tylko "solidne" obiekty
        if (obj.get() != this && obj->isSolid()) {
            // SFML 3: Używamy .findIntersection()! 
            // Zwraca wartość (czyli true), jeśli jest kolizja
            if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
                sprite.move({ -movement.x, 0.f }); // Cofamy ruch X
                break;
            }
        }
    }

    // --- OS Y ---
    sprite.move({ 0.f, movement.y }); // Przesuwamy tylko w pionie
    for (auto& obj : entities) {
        if (obj.get() != this && obj->isSolid()) {
           
            if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
                sprite.move({ 0.f, -movement.y }); 
                break;
            }
        }
    }

    updateAnimation();
}
void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}