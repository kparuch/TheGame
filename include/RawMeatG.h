#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>
#include <random>
class RawMeatG : public Pickup {
public:
	RawMeatG(float x, float y, const sf::Texture& texture);

	void applyEffect(Player* p) override;
};
