#include "Player.h"
#include "RawMeatG.h"
RawMeatG::RawMeatG (float x, float y, const sf::Texture &text) : Pickup(x,y,text){}
void RawMeatG::applyEffect(Player* p) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);

	int drop = dis(gen);
	if (drop == 0) {

		p->addHp(-20);
		p->speedUp(-5);
	}
	else {
	
		if (p->isCursed()) {
			return;
		}
		p->activateCurse();
	}
}