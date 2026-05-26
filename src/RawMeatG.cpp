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

	/*
	my favourite pick-up which took the longest to implement
	imagine benefits of raw meat multiplied by 10, what is the curse you may ask?
	... just don't stop moving ...
	*/
}
void RawMeatG::applyEffect(Enemy* e) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);
	int drop = dis(gen);
	if (drop == 0) {
		e->hpUp(-20);
		e->speedUp(-5);
	}
	else {
		e->speedUp(12.5);
		e->hpUp(2);
		e->addBombAmount(2);
		e->addBombRange(2, 2, 2, 2);
	}
}