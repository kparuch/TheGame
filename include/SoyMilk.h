#pragma once
#include "Pickup.h"
class SoyMilk : public Pickup {
public:
	SoyMilk(float x, float y, const sf::Texture& texture);
	void applyEffect(Player* p);
	void applyEffect(Enemy* e);
};
