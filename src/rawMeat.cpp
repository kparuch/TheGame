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
		p->speedUp(-0.15f);
		p->takeDamage(true);
		p->addBomb(1);
	}
	else {
		p->addHp(2);
		p->speedUp(0.75f);
		p->addBomb(1);
	}
}