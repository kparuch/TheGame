#pragma once
#include "Pickup.h"

class MythicalPchlarz : public Pickup {
public:MythicalPchlarz(float x, float y, const sf::Texture& texture);
	  void applyEffect(Player* p) override;
	  void applyEffect(Enemy* e) override;
};