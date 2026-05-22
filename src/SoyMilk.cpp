#include "SoyMilk.h"
#include "Player.h"

SoyMilk::SoyMilk(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void SoyMilk::applyEffect(Player* p) {
	p->addBombRange(+5, -1, -1, -1);
	p->speedUp(0.1f);
}