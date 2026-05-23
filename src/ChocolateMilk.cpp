#include "ChocolateMilk.h"
#include "Player.h"
ChocolateMilk::ChocolateMilk(float x, float y, const sf::Texture& texture) :Pickup(x, y, texture) {};
void ChocolateMilk::applyEffect(Player* p) {
	p->addHp(1);
	p->addBomb(1);
	p->addBombRange(1, 1, 1, 1);
	p->speedUp(1.f);

	//simply increases each stat, 1up for everything.
}