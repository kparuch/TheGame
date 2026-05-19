#include "Game.h"
#include "Wall.h"
#include <iostream>
#include "Floor.h"
#include "Crate.h"
#include "Player.h"
#include "algorithm"
#include "Pickup.h"
#include <random>
#include "Tinfoil.h"

Game::Game() {
	window.create(sf::VideoMode({ 1400,900 }), "Soyman II The Way of the Bomb ");
	window.setFramerateLimit(30);
	normalWallText.loadFromFile("assets/idWall.png");
	wallTexture = normalWallText;
	cursedWallSheet.loadFromFile("assets/trollBombsUpdated.png");
	tinfoilText.loadFromFile("assets/newTinfoilCap.png");
	if (!wallTexture.loadFromFile("assets/idWall.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!crateTexture.loadFromFile("assets/crate.png")) {
		std::cout << "error in idWall loading;\n";
	}
	if (!floorTexture.loadFromFile("assets/wall.png")) {
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
		if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()){
			if (gameOver && mousePressed->button == sf::Mouse::Button::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				restartGame();
			}
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
void Game::update() {
	
	for (uint16_t i = 0; i < gameObjs.size(); ++i) {
		gameObjs[i]->update(gameObjs);

		if (Player* p = dynamic_cast<Player*>(gameObjs[i].get())) {
			if (p->isDead()) {
				trigerCurse();
			}
		}
	} 

	
	std::vector<std::unique_ptr<Entity>> pendingdrops;
	for (auto& obj : gameObjs) {
		if (obj->isDestroyed()) {
			if (Crate* crate = dynamic_cast<Crate*>(obj.get())) {
				static std::random_device rd;
				static std::mt19937 gen(rd());
				std::uniform_int_distribution<> distrib(1, 100);

				int roll = distrib(gen);
				std::cout << "Wylosowano: " << roll << "\n"; 

				if (roll >= 80) { 
					float dropX = crate->getBounds().position.x;
					float dropY = crate->getBounds().position.y;

					pendingdrops.push_back(std::make_unique<TinFoil>(dropX, dropY, tinfoilText));
				}
			}
		}
	}

	
	for (auto& drop : pendingdrops) {
		gameObjs.push_back(std::move(drop));
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
void Game::trigerCurse() {
	if (curseTrig) return;
	curseTrig = true;
	gameOver = true;
	
	floorTexture.loadFromFile("assets/floor.png");
	crateTexture.loadFromFile("assets/rawMeat.png");
	int totRows = 5;
	int totCols = 5;
	int tileSize = 256;
	for (uint16_t i = 0; i < gameObjs.size(); ++i) {
		if (Wall* wall = dynamic_cast<Wall*>(gameObjs[i].get())) {
			int randCol = rand() % (totCols+1);
			int randRow = rand() % (totRows);

			sf::IntRect randFrame({ randCol * tileSize, randRow * tileSize }, { tileSize,tileSize });
			wall->setCrused(cursedWallSheet, randFrame);
		}
		

	}
	
}

void Game::restartGame() {
	curseTrig = false;
	gameOver = false;
	floorTexture.loadFromFile("assets/wall.png");
	crateTexture.loadFromFile("assets/crate.png");
	wallTexture = normalWallText;
	gameObjs.clear();
	loadLevel();
}