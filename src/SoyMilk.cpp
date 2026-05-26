#include "SoyMilk.h"
#include "Player.h"

SoyMilk::SoyMilk(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void SoyMilk::applyEffect(Player* p) {
	p->addBombRange(-3, -3, -5, -5);
	p->speedUp(-1.5f);
	p->setHp(2);
	
	
	/*
	yeah its a horrible thing to take 
	reduces range by 3 in left and right dirs and by 5 in up and down
	can be helpful after the curse?
	
	*/
}
void SoyMilk::applyEffect(Enemy* e) {
	e->addBombRange(-3, -3, -5, -5);
	e->speedUp(-1.5f);
	e->setHp(2);
}