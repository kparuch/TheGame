#include "Button.h"

Button::Button(float x, float y, float width, float height, const sf::Font& font, std::string btnText)
    : text(font, btnText, 56)
{
    shapeColor = sf::Color(0, 0, 0); 
    shapeHoverColor = sf::Color(136, 0, 21);

    textColor = sf::Color(136,0,21); 
    textHoverColor = sf::Color::Black; 

    outlineColor = sf::Color::Red; 
    outlineHoverColor = sf::Color::Red; 
  
    shape.setPosition({ x, y });
    shape.setSize({ width, height });
    shape.setFillColor(shapeColor);


    shape.setOutlineThickness(3.f);
    shape.setOutlineColor(outlineColor);

    text.setFillColor(textColor);

    
    sf::FloatRect textRec = text.getLocalBounds();
    text.setOrigin({
        textRec.position.x + textRec.size.x / 2.0f,
        textRec.position.y + textRec.size.y / 2.0f
        });

    text.setPosition({
        x + width / 2.0f,
        y + height / 2.0f
        });
}

void Button::update(const sf::RenderWindow& window) {
    
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

   
    if (shape.getGlobalBounds().contains(mousePos)) {
        shape.setFillColor(shapeHoverColor);
        shape.setOutlineColor(outlineHoverColor);
        text.setFillColor(textHoverColor);
    }
    else {
        shape.setFillColor(shapeColor);
        shape.setOutlineColor(outlineColor);
        text.setFillColor(textColor);
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked(const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (shape.getGlobalBounds().contains(mousePos)) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            return true;
        }
    }
    return false;
}