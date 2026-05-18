#include "Game.h"
#include "Wall.h"
#include <iostream>
#include "Floor.h"
#include "Crate.h"
#include "Player.h"
#include "algorithm"

Game::Game() {
	window.create(sf::VideoMode({ 1400,900 }), "Soyman II The Way of the Bomb ");
	window.setFramerateLimit(30);
	if (!wallTexture.loadFromFile("assets/idWall.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!crateTexture.loadFromFile("assets/crate.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!floorTexture.loadFromFile("assets/floor.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!playerTexture.loadFromFile("assets/chudPlayer.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!bombTexture.loadFromFile("assets/bomb.png")) {
		std::cout << "errr while loading bomb text\n";
	}
	if (!explosionTexture.loadFromFile("assets/boomver00.png")) {
		std::cout << "errr while loading bomb text\n";
	}
	loadLevel();
};
Game::~Game() {};
void Game::run() {
	while (window.isOpen()) {
		processEvent();
		render();
		update();
	}
};
void Game::processEvent() {
	while (const std::optional event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}
void Game::render() {
	window.clear();
	for (auto& obj : gameObjs) {
		obj->draw(window);
	}
	window.display();
}
void Game::update(){
	for (uint16_t i = 0; i < gameObjs.size(); ++i){
		gameObjs[i]->update(gameObjs);
	}
	gameObjs.erase(
		std::remove_if(gameObjs.begin(), gameObjs.end(),
			[](const std::unique_ptr<Entity>& obj) {return obj->isDestroyed();}
		), gameObjs.end()

	);
}
void Game::loadLevel() {
	//level 1 -> more to come 
	std::vector<std::string> map = {
		"#################",
		"#P  XXX X XXX XE#",
		"# #X#X#X#X#X#X# #",
		"# X XXX X XXX X #",
		"#X#X#X#X#X#X#X#X#",
		"#XXXXXXXXXXXXXXX#",
		"#XXXXXXXXXXXXXXX#",
		"#X#X#X#X#X#X#X#X#",
		"#XX XXX X XXX XX#",
		"# #X#X#X#X#X#X# #",
		"#E  XXX X XXX  E#",
		"#################",

	};
	float tileSize = 64.0f;
	float playerPosX = 0.f;
	float playerPosY = 0.f;
	bool playerDetected = false;
	for (int y = 0; y < map.size(); ++y) {
		for (int x = 0; x < map[y].size(); ++x) {
			float posX = x * tileSize;
			float posY = y * tileSize;

			gameObjs.push_back(std::make_unique<Floor>(posX, posY, floorTexture));
			if (map[y][x] == 'P') {
			
				playerPosX = posX;
				playerPosY = posY;
				playerDetected = true;
				std::cout << "Player detected at: " << playerDetected << " " << playerPosY << "\n";
			}
			if (map[y][x] == 'X') {
				gameObjs.push_back(std::make_unique<Crate>(posX, posY, crateTexture));
			}
			if (map[y][x] == '#') {
				gameObjs.push_back(std::make_unique<Wall>(posX, posY, wallTexture));
			}
		}
	}
	
	if (playerDetected) {
		gameObjs.push_back(std::make_unique<Player>(playerPosX, playerPosY, playerTexture, bombTexture, explosionTexture));
	}
}