#pragma once
#include "Pickup.h"

class BombPickup : public Pickup {
public:BombPickup(float x, float y, const sf::Texture& texture);
	  void applyEffect(Player* p) override;
};