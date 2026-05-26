#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>

class RawMeat : public Pickup {
public:

    RawMeat(float x, float y, const sf::Texture& texture);

    void applyEffect(Player* p) override;
    void applyEffect(Enemy* e) override;
};