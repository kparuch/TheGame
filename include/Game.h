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
	sf::Texture bombTexture;
	sf::Texture explosionTexture;
	std::vector<std::unique_ptr<Entity>>gameObjs;
	void loadLevel();
	bool gameOver = false;
	bool curseTrig = false;
	void restartGame();
	void trigerCurse();
	sf::Texture cursedWallSheet;
	sf::Texture normalWallText;
public:
	Game();
	virtual ~Game();
	void run();
};
