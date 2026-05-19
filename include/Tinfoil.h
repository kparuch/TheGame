#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>

class TinFoil : public Pickup {
public:
   
    TinFoil(float x, float y, const sf::Texture& texture);

    void applyEffect(Player* p) override;
};