#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <array>
#include "Entity.h"
#include "Pickup.h"
#include "TextureManager.h"
#include "Menu.h"
#include "Enemy.h"
#include <SFML/Audio.hpp>
#include "InfoScreen.h"
enum class GameState {
    Menu,
    LevelSelect,
    FreePlay,
    Story,
    Info,
    GameOver,
    Win,
    Exit
};

class Game {
private:
    sf::RenderWindow window;
    void update();
    void processEvent();
    void render();
    void renderLevelSelect();
    void checkWinCondition();

    std::vector<std::unique_ptr<Entity>> gameObjs;
    void loadLevel(int levelIndex, Difficulty diff);
    void restartGame();
    void trigerCurse();

    // --- Stan gry ---
    bool gameOver = false;
    bool curseTrig = false;

    // --- Level select / progresja ---
    int _currentLevel = 0;
    Difficulty _currentDifficulty = Difficulty::Normal;
    std::array<bool, 4> _levelUnlocked = { true, false, false, false };
    std::array<bool, 4> _levelBeatenHard = { false, false, false, false };
    bool _levelComplete = false;
    sf::Clock _levelCompleteTimer;
    void saveProgress();
    void loadProgress();
   
    TextureManager texHandler;
    GameState currentGameState = GameState::Menu;
    std::unique_ptr<MainMenu> mainMenu;

    sf::SoundBuffer explosionSound;
    sf::Font mainFont;
    sf::Text gameOverText;
    sf::SoundBuffer deathSoundBuf;
    sf::Sound deathSound;
    sf::Music bgMusic;
    std::vector<std::string> playlist;
    int currentTrackId = 0;
    void renderHUD();
    sf::Music menuMusic;
    std::unique_ptr<InfoScreen> infoScreen;
public:
    Game();
    virtual ~Game();
    void run();
    void updateMusic();
};