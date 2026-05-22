#include "TinFoil.h"
#include "Player.h"


TinFoil::TinFoil(float x, float y, const sf::Texture& texture)
    : Pickup(x, y, texture)
{
}
void TinFoil::applyEffect(Player* p) {
    p->addBombRange(1, 1, 1, 1);
}