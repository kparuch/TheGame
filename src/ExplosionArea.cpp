#include "ExplosionArea.h"
#include "Pickup.h"
ExplosionArea::ExplosionArea(float x, float y, const sf::Texture& texture, const sf::SoundBuffer& soundBuff, bool playSound)
    : sprite(texture), toBeErased(false), isVisible(true), sound(soundBuff)
{
    sprite.setPosition({ x, y });
    sprite.setScale({ 0.25f, 0.25f });
    sound.setBuffer(soundBuff);
    if (playSound) {
		sound.setVolume(50);
        sound.play();
    }
}
void ExplosionArea::update(std::vector<std::unique_ptr<Entity>>&entities) {
   if(isVisible) {
        if (czas.getElapsedTime().asSeconds() >= 0.6f) {
			isVisible = false;
            sprite.setColor(sf::Color(255, 255, 255, 0));
            sprite.setScale({ 0.f, 0.f });
        }
        else {
            for (auto& obj : entities) {
                if (Pickup* pickup = dynamic_cast<Pickup*>(obj.get())) {
                    if (sprite.getGlobalBounds().findIntersection(pickup->getBounds())) {
                        pickup->burn();
                    }
                }
            }
        }
    }
   
   
    if (!isVisible && sound.getStatus() == sf::Sound::Status::Stopped) {
        toBeErased = true;
    }
}

void ExplosionArea::draw(sf::RenderWindow& window) {
    if(isVisible){
        window.draw(sprite);
    }
};