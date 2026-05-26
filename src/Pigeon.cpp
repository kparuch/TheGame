#include "Pigeon.h"
#include "Player.h"

Pigeon::Pigeon(float x, float y, const sf::Texture& texture) :Pickup(x, y, texture) {};
void Pigeon::applyEffect(Player* p) {
	p->speedUp(0.65f);
	//pigeon is the best pickup, increases speed by a lot, and has no downsides, because pigeons are lovely birds <33
}
void Pigeon::applyEffect(Enemy* e) {
	e->speedUp(0.65f);
	//same effect for enemy, increases speed by a lot, because pigeons are lovely birds <33
}