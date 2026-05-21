#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Entity.h"
#include "Pickup.h"
#include "TextureManager.h"
class Game {
private:
	sf::RenderWindow window;
	void update();
	void processEvent();
	void render();
	std::vector<std::unique_ptr<Entity>>gameObjs;
	void loadLevel();
	bool gameOver = false;
	bool curseTrig = false;
	void restartGame();
	void trigerCurse();
	TextureManager texHandler;
public:
	Game();
	virtual ~Game();
	void run();
};
