#include "Game.h"
#include "Wall.h"
#include <iostream>
#include "Floor.h"
#include "Crate.h"
#include "Player.h"
#include "algorithm"
#include "Pickup.h"
#include <random>
#include "PickupFactory.h"
#include "Menu.h"
#include <memory>
Game::Game() {
	window.create(sf::VideoMode({ 1920,1080 }), "Soyman II The Way of the Bomb ");
	window.setFramerateLimit(30);
	
	//essentials
	texHandler.load("wall", "assets/idWall.png");
	texHandler.load("crate", "assets/crate.png");
	texHandler.load("floor", "assets/wall.png");		
	texHandler.load("normPlayer", "assets/chudPlayer.png");
	texHandler.load("cursedPlayer", "assets/cursedPlayer.png");
	texHandler.load("bomb", "assets/bomb.png");
	texHandler.load("explosion", "assets/boomver00.png");
	texHandler.load("menuBg", "assets/loadingBg.png");

	//curse
	texHandler.load("cursed_wall", "assets/trollBombsUpdated.png");
	texHandler.load("cursed_crate", "assets/rawMeat.png");
	texHandler.load("cursed_floor", "assets/floor.png");
	//pickups
	texHandler.load("tinfoil", "assets/newTinfoilCap.png");
	texHandler.load("rawMeat", "assets/rawMeat.png");
	texHandler.load("rawMeatG", "assets/rawMeatG.png");
	texHandler.load("chocoMilk", "assets/chocolateMilk.png");
	texHandler.load("soy", "assets/soyMilk.png");
	mainMenu = std::make_unique<MainMenu>(texHandler.get("menuBg"));
	//loadLevel();
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
		if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {

			if (currentGameState == GameState::Menu && mousePressed->button == sf::Mouse::Button::Left) {

				MenuState action = mainMenu->handleMouseClick(window);

				if (action == MenuState::Play) {
					currentGameState = GameState::FreePlay; 
					loadLevel();                            
				}
				else if (action == MenuState::Exit) {
					window.close();                         
				}
			}
		
		else if (gameOver && mousePressed->button == sf::Mouse::Button::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				restartGame();
			}
		}
	}
}
void Game::render() {
	window.clear();
	window.clear(sf::Color::Black);

	
	if (currentGameState == GameState::Menu) {
		if (mainMenu) {
			mainMenu->draw(window);
		}
	}
	else if (currentGameState == GameState::FreePlay) {
		for (auto& obj : gameObjs) {
			obj->draw(window);
		}
	}
	
	window.display();
}
void Game::update() {
	if (currentGameState == GameState::Menu) {
		if (mainMenu) {
			mainMenu->update(window); 
		}
		return; 
	}
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
				std::cout << "Rolled: " << roll << "\n"; 

				if (roll >= 70) { 
					float dropX = crate->getBounds().position.x;
					float dropY = crate->getBounds().position.y;
					auto newPickup = PickupFactory::spawnRandom(dropX + 2.f, dropY + 2.f, texHandler);

					if (newPickup != nullptr) {
						pendingdrops.push_back(std::move(newPickup));
					}
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
			//"#################",
			//"#P  XXX X XXX XE#",
			//"# #X#X#X#X#X#X# #",
			//"# X XXX X XXX X #",
			//"#X#X#X#X#X#X#X#X#",
			//"#XXXXXXXXXXXXXXX#",
			//"#XXXXXXXXXXXXXXX#",
			//"#X#X#X#X#X#X#X#X#",
			//"#XX XXX X XXX XX#",
			//"# #X#X#X#X#X#X# #",
			//"#E  XXX X XXX  E#",
			//"#################",
		"#########################",
	"#P  XX XXXXX XXXXX XX  E#",
	"# #X#X#X#X#X#X#X#X#X#X# #",
	"# XXXX XXXXX XXXXX XXXX #",
	"#X#X# #X#X#X#X#X#X# #X#X#",
	"# XXXXXXXX X XXXXXXXX X #",
	"#X#X#X#X#X# #X#X#X#X#X#X#",
	"# XXXX XXXXX XXXXX XXXX #",
	"#X#X#X# #X#X#X#X# #X#X#X#",
	"# XXXX XXXXX XXXXX XXXX #",
	"#X#X#X#X#X# #X#X#X#X#X#X#",
	"# XXXXXXXX X XXXXXXXX X #",
	"#X#X# #X#X#X#X#X#X# #X#X#",
	"# XXXX XXXXX XXXXX XXXX #",
	"# #X#X#X#X#X#X#X#X#X#X# #",
	"#E  XX XXXXX XXXXX XX  E#",
	"#########################"

	};
	float tileSize = 64.0f;
	float playerPosX = 0.f;
	float playerPosY = 0.f;
	bool playerDetected = false;
	for (int y = 0; y < map.size(); ++y) {
		for (int x = 0; x < map[y].size(); ++x) {
			float posX = x * tileSize;
			float posY = y * tileSize;

			gameObjs.push_back(std::make_unique<Floor>(posX, posY, texHandler.get("floor")));
			if (map[y][x] == 'P') {
			
				playerPosX = posX;
				playerPosY = posY;
				playerDetected = true;
				std::cout << "Player detected at: " << playerDetected << " " << playerPosY << "\n";
			}
			if (map[y][x] == 'X') {
				gameObjs.push_back(std::make_unique<Crate>(posX, posY, texHandler.get("crate")));
			}
			if (map[y][x] == '#') {
				gameObjs.push_back(std::make_unique<Wall>(posX, posY, texHandler.get("wall")));
			}
		}
	}
	
	if (playerDetected) {
		gameObjs.push_back(std::make_unique<Player>(playerPosX, playerPosY,
			texHandler.get("normPlayer"),
			texHandler.get("bomb"),
			texHandler.get("explosion"),
			texHandler.get("cursedPlayer")));
	}
}
void Game::trigerCurse() {
	if (curseTrig) return;
	curseTrig = true;
	gameOver = true;
	
	
	int totRows = 5;
	int totCols = 5;
	int tileSize = 256;
	for (uint16_t i = 0; i < gameObjs.size(); ++i) {
		if (Wall* wall = dynamic_cast<Wall*>(gameObjs[i].get())) {
			int randCol = rand() % (totCols+1);
			int randRow = rand() % (totRows);

			sf::IntRect randFrame({ randCol * tileSize, randRow * tileSize }, { tileSize,tileSize });
			wall->setCrused(texHandler.get("cursed_wall"), randFrame);
		}
		else if(Crate* crate = dynamic_cast<Crate*>(gameObjs[i].get())) {
			crate->setCursed(texHandler.get("cursed_crate"));
		}
		else if (Floor* floor = dynamic_cast<Floor*>(gameObjs[i].get())) {
			floor->setCursed(texHandler.get("cursed_floor"));
		}
		

	}
	
}

void Game::restartGame() {
	curseTrig = false;
	gameOver = false;
	gameObjs.clear();
	loadLevel();
}