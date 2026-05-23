#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>

class ChocolateMilkG : public Pickup {
public:
	ChocolateMilkG(float x, float y, const sf::Texture& texture);
	void applyEffect(Player* p);
};