#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Entity.h"
class Game {
private:
	sf::RenderWindow window;
	void update();
	void processEvent();
	void render();
	sf::Texture wallTexture;
	sf::Texture crateTexture;
	sf::Texture floorTexture;
	sf::Texture playerTexture;
	std::vector<std::unique_ptr<Entity>>gameObjs;
	void loadLevel();
public:
	Game();
	virtual ~Game();
	void run();
};
