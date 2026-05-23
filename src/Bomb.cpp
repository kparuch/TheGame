#include "Bomb.h"
#include "Wall.h"
#include "Crate.h"
#include "Player.h"
#include "ExplosionArea.h"
#include <cmath> 
#include <vector>

Bomb::Bomb(float x, float y, const sf::Texture& bombText, const sf::Texture& expText, BombStats stats, const sf::SoundBuffer &soundBuff)
    : sprite(bombText), explosionTexture(expText), isExploded(false), _stats(stats), isPassable(true), explosionSound(soundBuff)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.20f, 0.20f });
    
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
    if (isPassable) {
        bool isEntityIn = false;
        for(const auto&enty : entities){
            if (dynamic_cast<Player*>(enty.get())) {
                if (sprite.getGlobalBounds().findIntersection(enty->getBounds())) {
                    isEntityIn = true;
                    break;
                }
        }
        }
        if (!isEntityIn) {
            isPassable = false;
        }
    }
            
    updateAnimation(); 

    if (timer.getElapsedTime().asSeconds() >= 3.0f || forceExplode) {
        isExploded = true;
        sf::Vector2f center = sprite.getPosition();
        float tileSize = 64.0f;

        std::vector<sf::Vector2f> firePos;
        firePos.push_back(center);
        struct DirInfo {
            sf::Vector2i vec;
            int maxRange;
        };
        std::vector<DirInfo> dirs = {
    {{0, -1}, _stats.rangeUp},
    {{0, 1},  _stats.rangeDown},
    {{-1, 0}, _stats.rangeLeft},
    {{1, 0},  _stats.rangeRight}
        };

        

       
        for (const auto& dir : dirs) {
            for (int i = 1; i <= dir.maxRange; ++i) {
                float checkX = center.x + (dir.vec.x * i * tileSize);
                float checkY = center.y + (dir.vec.y * i * tileSize);
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
                        if (Bomb* otherBomb = dynamic_cast<Bomb*>(obj.get())) {
                            otherBomb->triggerExplosion();
                            stopFire = true;
                            spawnFire = false;
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
        bool isFirstTile = true;
        for (const auto& pos : firePos) {
            entities.push_back(std::make_unique<ExplosionArea>(pos.x, pos.y, explosionTexture, explosionSound, isFirstTile));
            isFirstTile = false;
        }
    }
}

void Bomb::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
