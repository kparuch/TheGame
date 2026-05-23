#include "BombPickup.h"
#include "Player.h"
BombPickup::BombPickup(float x, float y, const sf::Texture& texture) : Pickup(x, y, texture) {};
void BombPickup::applyEffect(Player* p) {
	p->addBomb(1);

	// as the name suggests?? increases bomb count by one and EXACTLY one , no more no less, 
	// not even a little bit more or less, just one bomb, the most basic pickup in the game, 
	// but still pretty useful if you ask me.
}