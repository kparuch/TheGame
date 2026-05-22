#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
private:
	sf::RectangleShape shape;
	sf::Text text;
	sf::Color shapeColor;
	sf::Color shapeHoverColor;
	sf::Color textColor;
	sf::Color textHoverColor;
	sf::Color outlineColor;
	sf::Color outlineHoverColor;
public:
	Button(float x, float y, float width, float height, const sf::Font& font, std::string btnText);
	void update(const sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	bool isClicked(const sf::RenderWindow& window);
};