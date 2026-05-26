#include "Menu.h"
#include <iostream>
#include <SFML/Audio.hpp>
MainMenu::MainMenu(const sf::Texture& bgTexture, const sf::Texture& sheetTexture) 
    : staticBg(bgTexture),
    buttonSheet(sheetTexture),
    playBtn(buttonSheet),  
    infoBtn(buttonSheet),  
    exitBtn(buttonSheet)
{
    staticBg.setScale({ 1.875f, 2.109375f });
    staticBg.setPosition({ 0.f, 0.f });
    menuMusic.openFromFile("assets/sounds/menuMusic.mp3");
    menuMusic.setLooping(true);
    menuMusic.play();
  
    sf::IntRect playDef({ 123, 67 }, { 279, 129 });   
    sf::IntRect playHov({ 414, 67 }, { 279, 129 }); 
   
    sf::IntRect playAct({ 180 + 660, 67 }, { 276, 121 }); //will likely be depracted

    
    sf::IntRect infoDef({ 123, 211 }, { 279, 129 });   
    sf::IntRect infoHov({ 414, 211 }, { 279, 129 });
    sf::IntRect infoAct({ 180 + 660, 187 }, { 310, 85 });  //will likely be depracted

    
    sf::IntRect exitDef({ 123, 355 }, { 279, 129 });
    sf::IntRect exitHov({ 414, 355 }, { 279, 129 });
	sf::IntRect exitAct({ 180 + 660, 400 }, { 310, 85 });  //will likely be depracted (as click animation is not planned, only hover)

    float btnScale = 2.0f;
    float startX = 1175.f; 

    setupButton(playBtn, startX, 230.f, MenuState::Play, playDef, playHov,playAct);
    setupButton(infoBtn, startX, 490.f, MenuState::Info, infoDef, infoHov, infoAct);
    setupButton(exitBtn, startX, 750.f, MenuState::Exit, exitDef, exitHov, exitAct);

    allButtons = { &playBtn, &infoBtn, &exitBtn };
    for (auto* btn : allButtons) {
        btn->sprite.setScale({ btnScale, btnScale });
    }
}
void MainMenu::setupButton(MenuButton& btn, float x, float y, MenuState action,
    sf::IntRect rectDefault, sf::IntRect rectHover, sf::IntRect rectActive)
{
    btn.sprite.setTexture(buttonSheet);
    btn.sprite.setTextureRect(rectDefault); 
    btn.sprite.setPosition({ x, y });

    
    btn.action = action;
    btn.textureRects[0] = rectDefault;
    btn.textureRects[1] = rectHover;
    btn.textureRects[2] = rectActive;
}

void MainMenu::update(const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto* btn : allButtons) {
 
        btn->hitbox = btn->sprite.getGlobalBounds();

       
        if (btn->hitbox.contains(mousePos)) {
            
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                btn->currentVisualState = ButtonVisualState::Active;
            }
            else {
                btn->currentVisualState = ButtonVisualState::Hover;
            }
        }
        else {
            
            btn->currentVisualState = ButtonVisualState::Default;
        }


        int stateIndex = static_cast<int>(btn->currentVisualState);
        btn->sprite.setTextureRect(btn->textureRects[stateIndex]);
    }
}

void MainMenu::draw(sf::RenderWindow& window) {
   
    window.draw(staticBg);

   
    for (auto* btn : allButtons) {
        window.draw(btn->sprite);
    }
}

MenuState MainMenu::handleMouseClick(const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (auto* btn : allButtons) {
        
        if (btn->hitbox.contains(mousePos)) {
            //clickSound.play(); //to be added later

            if (btn->action == MenuState::Play) {
                menuMusic.stop(); 
            }
            return btn->action;
        }
    }

    return MenuState::None;
}