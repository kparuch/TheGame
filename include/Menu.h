#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

enum class MenuState {
    None,
    Play,
    Info,
    Exit
};


enum class ButtonVisualState { Default, Hover, Active };

class MainMenu {
private:
    struct MenuButton {
        sf::Sprite sprite;
        sf::FloatRect hitbox;
        MenuState action;
        sf::IntRect textureRects[3];
        ButtonVisualState currentVisualState;

        MenuButton(const sf::Texture& tex)
            : sprite(tex), action(MenuState::None), currentVisualState(ButtonVisualState::Default) {
        }
    };

    sf::Texture buttonSheet;
    sf::Sprite staticBg;
    sf::Music menuMusic;

    MenuButton playBtn;
    MenuButton infoBtn;
    MenuButton exitBtn;

    std::vector<MenuButton*> allButtons;

    void setupButton(MenuButton& btn, float x, float y, MenuState action,
        sf::IntRect rectDefault, sf::IntRect rectHover, sf::IntRect rectActive);

public:
    MainMenu(const sf::Texture& bgTexture, const sf::Texture& sheetTexture);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    MenuState handleMouseClick(const sf::RenderWindow& window);
};