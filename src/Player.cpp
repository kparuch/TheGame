#include "Player.h"
#include "Bomb.h"
#include <cmath>
Player::Player(float x, float y, const sf::Texture& texture, const sf::Texture &bombTex, const sf::Texture &explTex)
    : sprite(texture), _speed(7.0f), currentState(PlayerState::Idle), bombTexRef(bombTex),explTexRef(explTex)
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
void Player::update( std::vector<std::unique_ptr<Entity>>& entities) {
    if (currentState == PlayerState::PlacingBomb) {
        if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
            currentState = PlayerState::Idle;
        }
    }
    sf::Vector2f movement({ 0.f, 0.f });
    bool isMoving = false;

   
    if (currentState != PlayerState::Dead) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { movement.y -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { movement.y += _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { movement.x -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { movement.x += _speed; isMoving = true; }
}
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        if (bombCooldown.getElapsedTime().asSeconds() > 3.0f) //na ten moment zakladamy ze gracz ma jedna bombe, przy robieniu powerupow bede modyfikowal metode stawiania bomby
        {
            float gridX = std::round(sprite.getPosition().x / 64.0f) * 64.0f;
            float gridY = std::round(sprite.getPosition().y / 64.0f) * 64.0f;
            currentState = PlayerState::PlacingBomb;
            actionTimer.restart();
            entities.push_back(std::make_unique<Bomb>(gridX, gridY, bombTexRef, explTexRef));
            bombCooldown.restart();

        }
    }
    if (currentState != PlayerState::Dead &&
        currentState != PlayerState::TakeDamage &&
        currentState != PlayerState::PlacingBomb)
    {
        if (isMoving) currentState = PlayerState::Moving;
        else currentState = PlayerState::Idle;
    }

    // --- OS X ---
    sprite.move({ movement.x, 0.f }); 
    for (auto& obj : entities) {
        
        if (obj.get() != this && obj->isSolid()) {
           
            if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
                sprite.move({ -movement.x, 0.f }); 
                break;
            }
        }
    }

    // --- OS Y ---
    sprite.move({ 0.f, movement.y }); 
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
