#include "RawMeat.h"
#include "Player.h"
#include <random>
RawMeat::RawMeat(float x, float y, const sf::Texture& texture) : 
	Pickup(x,y,texture){

}
void RawMeat::applyEffect(Player* p) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 5);
	
	int drop = dis(gen);
	if (drop < 4) {
		p->speedUp(-0.35f);
		p->takeDamage(true);
		p->addBomb(1);
	}
	else {
		p->addHp(2);
		p->speedUp(0.75f);
		p->addBomb(1);
	}
	
	/* 
	uhh I wouldn't eat raw meat personally, but since being primal is optimal, the raw meat pickup reflects its real life outcomes:

	either you get pretty bad food poisoning and lose hp, get slower and gain a bomb, or you get a small hp boost, a speed boost and a bomb.
	*/
}
void RawMeat::applyEffect(Enemy* e) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 5);

	int drop = dis(gen);
	if (drop < 2) {
		e->speedUp(-0.35f);
		e->takeDamage();

	}
	else {
		e->hpUp(2);
		e->speedUp(0.75f);
		e->addBombAmount(1);
	}
}