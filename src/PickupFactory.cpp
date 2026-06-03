#include "PickupFactory.h"
#include "Tinfoil.h"
#include "RawMeat.h"
#include "RawMeatG.h"
#include "ChocolateMilk.h"
#include "SoyMilk.h"
#include "ChocolateMilkG.h"
#include "Pigeon.h"
#include "BombPickup.h"
#include "BombBinary.h"
#include "Pchlarz.h" //in rememberance of Vincent the Claw... i miss you bud

#include "Impossible.h" 
/*
 
this pickup, no, it cannot be... its been gone for so long, but it seems like it has returned, the mythical pchlarz, 
the most powerful pickup in the game, its effects are unknown, but it is said that it can grant the player unimaginable power, but at a great cost...

*/

#include <random>

std::unique_ptr<Pickup>PickupFactory::spawnRandom(float x, float y, const TextureManager& tm) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::discrete_distribution<> dist({6, 4, 4, 4, 6, 4, 6, 4,6,1,3});
	int drop = dist(gen);
	switch (drop) {
	case 0: return std::make_unique<TinFoil>(x, y, tm.get("tinfoil"));
	case 1: return std::make_unique<RawMeat>(x, y, tm.get("rawMeat"));
	case 2: return std::make_unique<RawMeatG>(x, y, tm.get("rawMeatG"));
	case 3: return std::make_unique<ChocolateMilk>(x, y, tm.get("chocoMilk"));
	case 4: return std::make_unique<Pigeon>(x, y, tm.get("pigeon"));
	case 5: return std::make_unique<ChocolateMilkG>(x, y, tm.get("chocoMilkG"));
	case 6: return std::make_unique<BombPickup>(x, y, tm.get("bombUp"));
	case 7: return std::make_unique<BombBinary>(x, y, tm.get("???"));
	case 8: return std::make_unique<Pchlarz>(x, y, tm.get("pchlarz"));
	case 9: return std::make_unique<MythicalPchlarz>(x, y, tm.get("pchlarz?"));
	default: return std::make_unique<SoyMilk>(x,y,tm.get("soy"));
	}
}