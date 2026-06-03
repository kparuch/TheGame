#include "Pchlarz.h"
#include "Player.h"

Pchlarz::Pchlarz(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void Pchlarz::applyEffect(Player* p) {
	p->setCanThrow(true);
	p->addHp(1);
}
void Pchlarz::applyEffect(Enemy* e) {
	e->addBombAmount(1);
	e->hpUp(3);
	//enemy effect altered to make the game more fair towards new players
}