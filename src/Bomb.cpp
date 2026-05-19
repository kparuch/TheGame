#include "Bomb.h"
#include "Wall.h"
#include "Crate.h"
#include "Player.h"
#include "ExplosionArea.h"
#include <cmath> 

Bomb::Bomb(float x, float y, const sf::Texture& bombText, const sf::Texture& expText, int range)
    : sprite(bombText), explosionTexture(expText), isExploded(false), _range(range)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.25f, 0.25f });

    frameWidth = bombText.getSize().x / 2;  
    frameHeight = bombText.getSize().y / 3;

    updateAnimation();
}

void Bomb::updateAnimation() {
    float time = timer.getElapsedTime().asSeconds();
    int col = 0;
    int row = 0;

    if (time <= 0.5f) {
        currentBombState = BombState::Placed;
        col = 0;
        row = 0;
    }
    else if (time < 3.0f) {
        currentBombState = BombState::LightUp;
        row = 1;

        int currentFrame = static_cast<int>(time / 0.4f);
        col = currentFrame % 2;
    }
    else {
        currentBombState = BombState::Exploded;
        col = 1;
        row = 2;
    }

    sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight }));
}

void Bomb::update(std::vector<std::unique_ptr<Entity>>& entities) {
    if (isExploded) return;

    updateAnimation(); 

    if (timer.getElapsedTime().asSeconds() >= 3.0f) {
        isExploded = true;
        sf::Vector2f center = sprite.getPosition();
        float tileSize = 64.0f;

        std::vector<sf::Vector2f> firePos;
        firePos.push_back(center);

        sf::Vector2i dirs[] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

       
        for (const auto& dir : dirs) {
            for (int i = 1; i <= _range; ++i) {
                float checkX = center.x + (dir.x * i * tileSize);
                float checkY = center.y + (dir.y * i * tileSize);
                sf::FloatRect currentTileRect({ checkX, checkY }, { tileSize, tileSize });

                bool stopFire = false;
                bool spawnFire = true;

                for (auto& obj : entities) {
                    if (obj.get() == this) continue;

                    if (currentTileRect.findIntersection(obj->getBounds())) {
                        if (dynamic_cast<Wall*>(obj.get())) {
                            stopFire = true;
                            spawnFire = false;
                            break;
                        }
                        if (dynamic_cast<Crate*>(obj.get())) {
                            obj->destroy();
                            stopFire = true;
                            spawnFire = true; 
                            break;
                        }
                    }
                }

                if (spawnFire) {
                    firePos.push_back({ checkX, checkY });
                }
                if (stopFire) {
                    break;
                }
            }
        } 

        for (const auto& pos : firePos) {
            entities.push_back(std::make_unique<ExplosionArea>(pos.x, pos.y, explosionTexture));
        }
    }
}

void Bomb::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}