#pragma once
#include <SFML/Graphics.hpp>
#include "Pickup.h"
#include <random>
#include "TextureManager.h"
class PickupFactory {
public:
	static std::unique_ptr<Pickup> spawnRandom(float x, float y, const TextureManager& tm);
};