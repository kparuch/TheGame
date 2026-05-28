#pragma once
#include <SFML/Graphics.hpp>

class InfoScreen {
public:
    InfoScreen(sf::Font& font);
    void draw(sf::RenderWindow& window);

private:
    sf::Font& font;
};