#pragma once
#include "Pickup.h"
#include <SFML/Graphics.hpp>

class Pigeon : public Pickup {
public:
	Pigeon(float x, float y, const sf::Texture& texture);
	void applyEffect(Player* p);
};