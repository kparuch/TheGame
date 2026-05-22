#include "ChocolateMilk.h"
#include "Player.h"
ChocolateMilk::ChocolateMilk(float x, float y, const sf::Texture& texture) :Pickup(x, y, texture) {};
void ChocolateMilk::applyEffect(Player* p) {
	p->addHp(2);
	p->addBomb(2);
	p->addBombRange(3, 3, -2, -2);
	p->speedUp(1.6f);
}