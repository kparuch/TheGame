#include "PickupFactory.h"
#include "Tinfoil.h"
#include "RawMeat.h"
#include "RawMeatG.h"
#include "ChocolateMilk.h"
#include "SoyMilk.h"
#include <random>

std::unique_ptr<Pickup>PickupFactory::spawnRandom(float x, float y, const TextureManager& tm) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::discrete_distribution<> dist({ 7, 2, 1, 1 ,2});
	int drop = dist(gen);
	switch (drop) {
	case 0: return std::make_unique<TinFoil>(x, y, tm.get("tinfoil"));
	case 1: return std::make_unique<RawMeat>(x, y, tm.get("rawMeat"));
	case 2: return std::make_unique<RawMeatG>(x, y, tm.get("rawMeatG"));
	case 3: return std::make_unique<ChocolateMilk>(x, y, tm.get("chocoMilk"));
	default: return std::make_unique<SoyMilk>(x,y,tm.get("soy"));
	}
}