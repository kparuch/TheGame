#include "Impossible.h"
#include "Player.h"
MythicalPchlarz::MythicalPchlarz(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void MythicalPchlarz::applyEffect(Player* p) {
	p->addBombDamage(1);
	p->addHp(9);
	p->speedUp(3.f);

	//impossible... but Vincent has been gone for so long, I truly miss him, thus his remembrance will be the most powerful pickup in the game, granting a huge hp boost, a huge speed boost and a bomb damage increase, truly a mythical item, only for the most worthy of players
	//can break curses, silent one, look for it
}
void MythicalPchlarz::applyEffect(Enemy* e) {
	e->addBombRange(-1, -1, -1, -1);
	e->hpUp(-1);
	e->speedUp(-1.f);
}