#include "Menu.h"
#include <iostream>
MainMenu::MainMenu(const sf::Texture& bgTexture) : bgSprite(bgTexture){
	bgSprite.setScale({ 1.5f, 1.5f });
	bgSprite.setPosition({ 0.f, 120.f });
	if (!font.openFromFile("assets/font/castellarReg.ttf")) {
		std::cerr << "Blad ladowania czcionki Castellar\n";
	}
	playBtn = std::make_unique<Button>(1200.f, 300.f, 450.f, 120.f, font, "PLAY");
	infoBtn = std::make_unique<Button>(1200.f, 500.f, 450.f, 120.f, font, "INFO");
	exitBtn = std::make_unique<Button>(1200.f, 700.f, 450.f, 120.f, font, "EXIT");
}
void MainMenu::update(const sf::RenderWindow& window) {
    playBtn->update(window);
    infoBtn->update(window);
    exitBtn->update(window);
}

void MainMenu::draw(sf::RenderWindow& window) {
    window.draw(bgSprite);
    playBtn->draw(window);
    infoBtn->draw(window);
    exitBtn->draw(window);
}

MenuState MainMenu::handleMouseClick(const sf::RenderWindow& window) {
    if (playBtn->isClicked(window)) return MenuState::Play;
    if (infoBtn->isClicked(window)) return MenuState::Info;
    if (exitBtn->isClicked(window)) return MenuState::Exit;

    return MenuState::None;
}