#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Entity.h"
#include "Pickup.h"
#include "TextureManager.h"
#include "Menu.h"
#include <SFML/Audio.hpp>
enum class GameState {
	Menu,
	Play,
	FreePlay,
	Story,
	Info,
	GameOver,
	Exit
};
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
	GameState currentGameState=GameState::Menu;
	std::unique_ptr<MainMenu>mainMenu;
	sf::SoundBuffer explosionSound;
	sf::Font mainFont;
	sf::Text gameOverText;
	
	sf::SoundBuffer deathSoundBuf;
	sf::Sound deathSound;
	sf::Music bgMusic;
	std::vector<std::string> playlist;
	int currentTrackId = 0;
	
public:
	Game();
	virtual ~Game();
	void run();
	void updateMusic();
};
