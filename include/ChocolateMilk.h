#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>

class ChocolateMilk : public Pickup {
public:
	ChocolateMilk(float x, float y, const sf::Texture& texture);
	void applyEffect(Player* p);
	void applyEffect(Enemy* e);
};