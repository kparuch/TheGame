#pragma once
#include "Pickup.h"

class Pchlarz : public Pickup {
public:Pchlarz(float x, float y, const sf::Texture& texture);
	  void applyEffect(Player* p) override;
	  void applyEffect(Enemy* e) override;
};