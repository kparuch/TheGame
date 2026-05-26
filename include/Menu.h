#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Button.h"
#include <SFML/Audio.hpp>
enum class MenuState {
	None,
	Play,
	Info,
	Exit
};
class MainMenu {
private:
	sf::Sprite bgSprite;
	sf::Font font;
	std::unique_ptr<Button>playBtn;
	std::unique_ptr<Button>infoBtn;
	std::unique_ptr<Button>exitBtn;
	sf::Music menuMusic;
	//sf::SoundBuffer clickSoundBuf;
	//sf::Sound clickSound;
	
public:
	MainMenu(const sf::Texture& bgTexture);
	void update(const sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	MenuState handleMouseClick(const sf::RenderWindow& window);
};