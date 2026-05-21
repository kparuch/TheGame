#include "Player.h"
#include "Bomb.h"
#include <cmath>
#include "ExplosionArea.h"
#include "Crate.h"
#include "Pickup.h"
#include <iostream>
Player::Player(float x, float y, const sf::Texture& normtexture, const sf::Texture &bombTex, const sf::Texture &explTex, const sf::Texture &curse)
    :sprite(normtexture), currentState(PlayerState::Idle), bombTexRef(bombTex),explTexRef(explTex), curseTex(curse)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.18f, 0.18f });
    frameWidth = normtexture.getSize().x / 2;
    frameHeight = normtexture.getSize().y / 3;
    normTex = normtexture;
    
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
void Player::activateCurse() {
    _isCursed = true;
    _hasDebuff = false;
    _ogSpeed = _speed;
    speedUp(20);
    _hp = 100;
    curseTime.restart();
    idleTimer.restart();
    lastPos = getBounds().position;
}
void Player::update( std::vector<std::unique_ptr<Entity>>& entities) {
    if (_isCursed) {
        
        
        sf::Vector2f currentPos = getBounds().position;
        bool isMoving = (currentPos != lastPos);
        if (isMoving) {
            idleTimer.restart();
        }
        else {
            if (idleTimer.getElapsedTime().asSeconds() >= 2.0f) {
                _isCursed = false;
                setHp(1);
                takeDamage(true);
                return;
            }
        }

        
        if (curseTime.getElapsedTime().asSeconds() >= 8.0f) {
            _isCursed = false;
            _hasDebuff = true;
            _speed = 1.5f;
            _hp = 1;
            debuffTime.restart();  
            sprite.setTexture(normTex);
            return;
        }

        
        sprite.setTexture(curseTex);

        if (curseTime.getElapsedTime().asSeconds() >= 4.0f) {
            ;
      

            if ((int)(curseTime.getElapsedTime().asSeconds() * 2.0f) % 2 == 0) {
                curseState = CursedState::Hit;
                
            }
            else {
                curseState = CursedState::PostHit;
                
            }
        }
        else {

       
            if (isMoving) {
                curseState = CursedState::Move;
            }
            else {
                curseState = CursedState::Idle;
            }
        }

        
        int col{}; int row{};
        int frameHeight = curseTex.getSize().y/2;
        int frameWidth = curseTex.getSize().x / 2;
        switch (curseState) {
        case CursedState::Idle:    col = 0; row = 0; break;
        case CursedState::Move:    col = 0; row = 1; break;
        case CursedState::Hit:     col = 1; row = 0; break;
        case CursedState::PostHit: col = 1; row = 1; break;
        }
        sprite.setScale({ 0.18f,0.18f });
        sprite.setTextureRect(sf::IntRect({ col * frameWidth, row * frameHeight }, { frameWidth, frameHeight }));
        sprite.setColor(sf::Color(255, 99, 71));
        lastPos = currentPos;
        
    }
    
    else if (_hasDebuff) {
        if (debuffTime.getElapsedTime().asSeconds() >= 5.0f) {
            _speed = _ogSpeed; 
            _hasDebuff = false;
            _hp = 1;
        }
        sprite.setTexture(normTex); 
        sprite.setColor(sf::Color(150, 150, 150));
        
    }
    else {
        sprite.setTexture(normTex);
        sprite.setColor(sf::Color::White);
    }
    if (currentState == PlayerState::TakeDamage) {
        if (actionTimer.getElapsedTime().asSeconds()>actionDurr) {
            currentState = PlayerState::Idle;
        }
    }
    if (currentState == PlayerState::PlacingBomb) {
        if (actionTimer.getElapsedTime().asSeconds() > actionDurr) {
            currentState = PlayerState::Idle;
        }
    }
    if (currentState != PlayerState::Dead && currentState != PlayerState::TakeDamage) {
        for (auto& obj : entities) {
            if (dynamic_cast<ExplosionArea*>(obj.get())) {
                if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
                    takeDamage();
                    break;
                }
            }
        }
    }
    sf::Vector2f movement({ 0.f, 0.f });
    bool isMoving = false;

   
    if (currentState != PlayerState::Dead) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { movement.y -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { movement.y += _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { movement.x -= _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { movement.x += _speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) { takeDamage(); }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !_isCursed) {
        if (bombCooldown.getElapsedTime().asSeconds() > 0.5f)
        {
            int activeBombs{};
            for (const auto& obj : entities) {
                if (dynamic_cast<Bomb*>(obj.get())) {
                    activeBombs++;
                }
            }
            if (activeBombs < _bombAmount) {
                float gridX = std::round(sprite.getPosition().x / 64.0f) * 64.0f;
                float gridY = std::round(sprite.getPosition().y / 64.0f) * 64.0f;
                currentState = PlayerState::PlacingBomb;
                actionTimer.restart();
                entities.push_back(std::make_unique<Bomb>(gridX, gridY, bombTexRef, explTexRef,  _currentBombStats));
                bombCooldown.restart();
            }


        }
    }
    }   
    
    if (currentState != PlayerState::Dead &&
        currentState != PlayerState::TakeDamage &&
        currentState != PlayerState::PlacingBomb)
    {
        if (isMoving) currentState = PlayerState::Moving;
        else currentState = PlayerState::Idle;
    }

   
    // --- RUCH W OSI X ---
    sprite.move({ movement.x, 0.f });
    for (auto& obj : entities) {
        if (obj.get() == this) continue;

        if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
            if (_isCursed) {
                if (Crate* crate = dynamic_cast<Crate*>(obj.get())) {
                    if (crate->isDestroyed()) continue;
                    crate->destroy();


                    continue;
                }
                else if (Pickup* pickup = dynamic_cast<Pickup*>(obj.get())) {
                    if (pickup->isDestroyed()) continue;

                    pickup->destroy();
                    continue;
                }
            }
            if (obj->isSolid()) {
                sprite.move({ -movement.x, 0.f });
                break;
            }
        }
    }

    
    sprite.move({ 0.f, movement.y });
    for (auto& obj : entities) {
        if (obj.get() == this) continue;

        if (sprite.getGlobalBounds().findIntersection(obj->getBounds())) {
            if (_isCursed) {
                if (Crate* crate = dynamic_cast<Crate*>(obj.get())) {
                    if (crate->isDestroyed()) continue;
                    crate->destroy();
                    

                    continue;
                }
                else if (Pickup* pickup = dynamic_cast<Pickup*>(obj.get())) {
                    if (pickup->isDestroyed()) continue;

                    pickup->destroy();
                    continue;
                }
            }
            if (obj->isSolid()) {
                sprite.move({ 0.f, -movement.y });
                break;
            }
        }
    }
    if (!_isCursed) {
        updateAnimation();
    }
   
}
void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
void Player::takeDamage(bool byPassGracePeriod) {
    if (currentState == PlayerState::Dead) {
        return;
    }
    if(currentState==PlayerState::TakeDamage && !byPassGracePeriod){
        return;
    }
    _hp--;
    if (_hp <= 0) {
        currentState = PlayerState::Dead;
    }
    else {
        currentState = PlayerState::TakeDamage;
        actionTimer.restart();
    }
}
void Player::addHp(int amount) {
    _hp += amount;
    if (_hp < 1) {
        _hp = 1;
    }
}

void Player::addBomb(int amount) {
    _bombAmount += amount;
    if (_bombAmount < 1) {
        _bombAmount = 1;
    }
}

void Player::speedUp(float val) {
    _speed += val;
    if (_speed < 4.0f) {
        _speed = 4.0f;
    }
    else if (_speed > 10.5f) {
        _speed = 10.5f;
    }
}

void Player::setHp(int val) {
    _hp = val;
}

void Player::setAnimState(CursedState state) {
    curseState = state;
    if (state == CursedState::Hit) {
        hitAnimTimer.restart();
    }
}
void Player::addBombRange(int left, int right, int up, int down) {
    _currentBombStats.rangeLeft += left;
    if (_currentBombStats.rangeLeft < 1) {
        _currentBombStats.rangeLeft=1;
    }
    _currentBombStats.rangeRight += right;
    if (_currentBombStats.rangeRight  < 1) {
        _currentBombStats.rangeRight=1;
    }
    _currentBombStats.rangeUp += up;
    if (_currentBombStats.rangeUp < 1) {
        _currentBombStats.rangeUp=1;
    }
    _currentBombStats.rangeDown += down;
    if (_currentBombStats.rangeDown  < 1) {
        _currentBombStats.rangeDown = 1;
    }
}