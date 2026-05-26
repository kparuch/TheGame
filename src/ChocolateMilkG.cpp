#include "ChocolateMilkG.h"
#include "Player.h"
ChocolateMilkG::ChocolateMilkG(float x, float y, const sf::Texture& texture) :Pickup(x, y, texture) {};
void ChocolateMilkG::applyEffect(Player* p) {
	p->addHp(2);
	p->addBomb(2);
	p->speedUp(2.f);
	//p->canKickBomb()

	//godtier version of chocolate milk,  huge speed boost, +2 bombs and hp, can kick bombs (not implemented yet)
}
void ChocolateMilkG::applyEffect(Enemy* e) {
	e->hpUp(2);
	e->addBombAmount(2);
	e->speedUp(2.f);
	//same effect for enemy, huge speed boost, +2 bombs and hp
}