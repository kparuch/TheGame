#include "BombBinary.h"
#include "Player.h"
BombBinary::BombBinary(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void BombBinary::applyEffect(Player* p) {
	p->addBomb(1);
	p->setHasLaser(true);
	p->addBombRange(3, 3, 3, 3);
}
void BombBinary::applyEffect(Enemy* e) {
	e->addBombAmount(1);
	//enemy effect altered to make the game more fair towards new players
}