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
#include "Enemy.h"
#include <fstream>
Game::Game(): gameOverText(mainFont, "GAME OVER", 120),deathSound(deathSoundBuf) {
	window.create(sf::VideoMode({ 1920,1080 }), "Soyman II The Way of the Bomb ");
	window.setFramerateLimit(30);
	//playlist for nice bg music :vv
	playlist = {
		"assets/sounds/bgSound01.mp3",
		"assets/sounds/bgSound02.mp3",
		"assets/sounds/bgSound03.mp3",
		"assets/sounds/bgSound04.mp3",
		"assets/sounds/bgSound05.mp3",
		"assets/sounds/bgSound06.mp3",
		"assets/sounds/bgSound010.mp3",
		"assets/sounds/bgSound011.mp3",
	};
	if (!playlist.empty()) {
		bgMusic.openFromFile(playlist[0]);
		bgMusic.setVolume(18.5f); 
	}
	explosionSound.loadFromFile("assets/sounds/boom.mp3");
	//essentials
	texHandler.load("wall", "assets/idWall.png");
	texHandler.load("crate", "assets/crate.png");
	texHandler.load("floor", "assets/wall.png");		
	texHandler.load("normPlayer", "assets/chudPlayer.png");
	texHandler.load("cursedPlayer", "assets/cursedPlayer.png");
	texHandler.load("bomb", "assets/bomb.png");
	texHandler.load("explosion", "assets/boomver00.png");
	texHandler.load("menuBg", "assets/menuBgVer3.png");
	texHandler.load("menuBtns", "assets/menuBtnsFixed.png");
	texHandler.load("enemy", "assets/enemyVer2.png");
	mainFont.openFromFile("assets/font/castellarReg.ttf");

	menuMusic.openFromFile("assets/sounds/menuMusic.mp3");
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
	texHandler.load("pigeon", "assets/pigeonUp.png");
	texHandler.load("chocoMilkG", "assets/MilchG.png");
	texHandler.load("bombUp", "assets/fixBombUp.png");
	texHandler.load("???", "assets/errorUp.png");
	mainMenu = std::make_unique<MainMenu>(texHandler.get("menuBg"), texHandler.get("menuBtns"));
	infoScreen = std::make_unique<InfoScreen>(mainFont);
	//essentials2
	gameOverText.setFont(mainFont);
	 
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setOutlineColor(sf::Color::Black);
	gameOverText.setOutlineThickness(5.f);
	deathSoundBuf.loadFromFile("assets/sounds/death.mp3");
	deathSound.setBuffer(deathSoundBuf);

	sf::FloatRect textRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin({
		textRect.position.x + textRect.size.x / 2.0f,
		textRect.position.y + textRect.size.y / 2.0f
		});

	gameOverText.setPosition({ 1920.f / 2.0f, 1080.f / 2.0f });

	Game::loadProgress();


};
Game::~Game() {};
void Game::run() {
	while (window.isOpen()) {
		processEvent();
		render();
		update();
	}
};
void Game::updateMusic() {

	if (currentGameState == GameState::FreePlay && !gameOver) {
		if (bgMusic.getStatus() == sf::Music::Status::Stopped) {
			currentTrackId++;
			if (currentTrackId >= playlist.size()) {
				currentTrackId = 0;
			}
			bgMusic.openFromFile(playlist[currentTrackId]);
			bgMusic.setVolume(25.f);
			bgMusic.play();
		}
	}
	else if (gameOver && bgMusic.getStatus() == sf::Music::Status::Playing) {
		bgMusic.stop();
	}
}
	



void Game::processEvent() {
	while (const std::optional event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		if (currentGameState == GameState::Info) {
			if (event->is<sf::Event::MouseButtonPressed>())
				currentGameState = GameState::Menu;
			continue;  
		}
		const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>();
		if (!mousePressed || mousePressed->button != sf::Mouse::Button::Left) continue;

		sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		// --- MENU ---
		if (currentGameState == GameState::Menu) {
			MenuState action = mainMenu->handleMouseClick(window);
			if (action == MenuState::Play) currentGameState = GameState::LevelSelect;
			else if (action == MenuState::Info) currentGameState = GameState::Info;

			
			else if (currentGameState == GameState::Info) {
				currentGameState = GameState::Menu;
			}
			else if (action == MenuState::Exit) window.close();
			menuMusic.play();
			menuMusic.setLooping(true);
			menuMusic.setVolume(22.5f);

		}


		// --- LEVEL SELECT ---
		else if (currentGameState == GameState::LevelSelect) {
			// level buttons
			const float btnW = 280.f, btnH = 120.f;
			const float startX = 960.f - (btnW * 1.5f + 60.f);
			const float btnY = 420.f;

			for (int i = 0; i < 3; ++i) {
				if (!_levelUnlocked[i]) continue;
				float x = startX + i * (btnW + 60.f);
				if (sf::FloatRect({ x, btnY }, { btnW, btnH }).contains(mouse)) {
					_currentLevel = i;
					gameObjs.clear();
					menuMusic.stop();
					loadLevel(_currentLevel, _currentDifficulty);
					currentGameState = GameState::FreePlay;
					return;
				}
			}

			// Difficulty toggle
			const float diffY = 655.f;
			if (sf::FloatRect({ 780.f, diffY }, { 180.f, 60.f }).contains(mouse))
				_currentDifficulty = Difficulty::Normal;
			if (sf::FloatRect({ 1000.f, diffY }, { 180.f, 60.f }).contains(mouse))
				_currentDifficulty = Difficulty::Hard;

			// return to menu
			if (sf::FloatRect({ 810.f, 860.f }, { 300.f, 60.f }).contains(mouse))
				currentGameState = GameState::Menu;
			// Secret level click
			if (_levelUnlocked[3]) {
				float sx = 960.f - 140.f;  // 820.f
				float sy = 220.f;           

				if (sf::FloatRect({ sx, sy }, { 280.f, 100.f }).contains(mouse)) {
					_currentLevel = 3;
					gameObjs.clear();
					loadLevel(3, _currentDifficulty);
					currentGameState = GameState::FreePlay;
					return;
				}
			}
			
		}

		// --- GAME OVER ---
		else if (gameOver) {
			// Retry
			if (sf::FloatRect({ 640.f, 630.f }, { 260.f, 70.f }).contains(mouse))
				restartGame();

			// Main Menu
			if (sf::FloatRect({ 1020.f, 630.f }, { 260.f, 70.f }).contains(mouse)) {
				curseTrig = false;
				gameOver = false;
				deathSound.stop();
				bgMusic.stop();
				menuMusic.play();
				menuMusic.setLooping(true);
				menuMusic.setVolume(22.5f);
				gameObjs.clear();
				currentGameState = GameState::Menu;
			}
		}
	}
}
void Game::renderLevelSelect() {
	window.clear(sf::Color(0, 0, 0));

	sf::Text title(mainFont, "SELECT LEVEL", 72);
	title.setFillColor(sf::Color::White);
	sf::FloatRect tr = title.getLocalBounds();
	title.setOrigin({ tr.position.x + tr.size.x / 2.f, tr.position.y + tr.size.y / 2.f });
	title.setPosition({ 960.f, 150.f });
	window.draw(title);

	//btns of levels
	const std::array<std::string, 3> labels = { "LEVEL 1", "LEVEL 2", "LEVEL 3" };
	const float btnW = 280.f, btnH = 120.f;
	const float startX = 960.f - (btnW * 1.5f + 60.f);
	const float btnY = 420.f;

	for (int i = 0; i < 3; ++i) {
		float x = startX + i * (btnW + 60.f);
		bool locked = !_levelUnlocked[i];

		sf::RectangleShape btn({ btnW, btnH });
		btn.setPosition({ x, btnY });
		btn.setFillColor(locked ? sf::Color(60, 60, 60) : sf::Color(50, 100, 50));
		btn.setOutlineColor(sf::Color::White);
		btn.setOutlineThickness(2.f);
		window.draw(btn);

		std::string label = labels[i];
		if (locked)            label += "\n[LOCKED]";
		else if (_levelBeatenHard[i]) label += "\n[MASTERED]";

		sf::Text btnText(mainFont, label, 28);
		btnText.setFillColor(locked ? sf::Color(120, 120, 120) : sf::Color::White);
		sf::FloatRect br = btnText.getLocalBounds();
		btnText.setOrigin({ br.position.x + br.size.x / 2.f, br.position.y + br.size.y / 2.f });
		btnText.setPosition({ x + btnW / 2.f, btnY + btnH / 2.f });
		window.draw(btnText);
	}
	// Secret level
	if (_levelUnlocked[3]) {
		bool locked = false;  
		float sx = 960.f - 140.f;
		float sy = 220.f;

		sf::RectangleShape sBtn({ 280.f, 100.f });
		sBtn.setPosition({ sx, sy });
		sBtn.setFillColor(sf::Color(100, 0, 120));
		sBtn.setOutlineColor(sf::Color(220, 0, 255));
		sBtn.setOutlineThickness(3.f);
		window.draw(sBtn);

		sf::Text sTxt(mainFont, "4/3", 26);
		sTxt.setFillColor(sf::Color(255, 200, 255));
		sf::FloatRect sr = sTxt.getLocalBounds();
		sTxt.setOrigin({ sr.position.x + sr.size.x / 2.f,
						 sr.position.y + sr.size.y / 2.f });
		sTxt.setPosition({ sx + 140.f, sy + 50.f });
		window.draw(sTxt);
	}

	// Difficulty toggle
	const float diffY = 600.f;
	const std::array<std::string, 2> diffLabels = { "NORMAL", "HARD" };
	const std::array<Difficulty, 2> diffs = { Difficulty::Normal, Difficulty::Hard };

	sf::Text diffTitle(mainFont, "DIFFICULTY:", 36);
	sf::FloatRect dtr = diffTitle.getLocalBounds();
	diffTitle.setOrigin({ dtr.position.x + dtr.size.x / 2.f, 0.f });
	diffTitle.setPosition({ 960.f, diffY });
	diffTitle.setFillColor(sf::Color::White);
	window.draw(diffTitle);

	for (int i = 0; i < 2; ++i) {
		float x = 780.f + i * 220.f;
		bool selected = (_currentDifficulty == diffs[i]);
		if (i == 1)
		{
			sf::RectangleShape btn({ 180.f, 60.f });
			btn.setPosition({ x, diffY + 55.f });
			btn.setFillColor(selected ? sf::Color(136, 0, 21) : sf::Color(60, 60, 60));
			btn.setOutlineColor(sf::Color::White);
			btn.setOutlineThickness(2.f);
			window.draw(btn);

			sf::Text t(mainFont, diffLabels[i], 28);
			t.setFillColor(sf::Color::White);
			sf::FloatRect tr2 = t.getLocalBounds();
			t.setOrigin({ tr2.position.x + tr2.size.x / 2.f, tr2.position.y + tr2.size.y / 2.f });
			t.setPosition({ x + 90.f, diffY + 85.f });
			window.draw(t);
		}
		else
		
		{
			sf::RectangleShape btn({ 180.f, 60.f });
			btn.setPosition({ x, diffY + 55.f });
			btn.setFillColor(selected ? sf::Color(180, 140, 21) : sf::Color(60, 60, 60));
			btn.setOutlineColor(sf::Color::White);
			btn.setOutlineThickness(2.f);
			window.draw(btn);

			sf::Text t(mainFont, diffLabels[i], 28);
			t.setFillColor(sf::Color::White);
			sf::FloatRect tr2 = t.getLocalBounds();
			t.setOrigin({ tr2.position.x + tr2.size.x / 2.f, tr2.position.y + tr2.size.y / 2.f });
			t.setPosition({ x + 90.f, diffY + 85.f });
			window.draw(t);
	}

	
	}

	//  Unlocking condition
	if (_currentLevel >= 0) {
		sf::Text hint(mainFont, "Complete the current level on HARD Difficulty to unlock the next one", 22);
		hint.setFillColor(sf::Color(180, 180, 180));
		sf::FloatRect hr = hint.getLocalBounds();
		hint.setOrigin({ hr.position.x + hr.size.x / 2.f, 0.f });
		hint.setPosition({ 960.f, 750.f });
		window.draw(hint);
	}
	// return btns
	sf::RectangleShape backBtn({ 300.f, 60.f });
	backBtn.setPosition({ 810.f, 860.f });
	backBtn.setFillColor(sf::Color(80, 40, 40));
	backBtn.setOutlineColor(sf::Color::White);
	backBtn.setOutlineThickness(2.f);
	window.draw(backBtn);

	sf::Text backText(mainFont, "MAIN MENU", 28);
	backText.setFillColor(sf::Color::White);
	sf::FloatRect btr = backText.getLocalBounds();
	backText.setOrigin({ btr.position.x + btr.size.x / 2.f, btr.position.y + btr.size.y / 2.f });
	backText.setPosition({ 960.f, 890.f });
	window.draw(backText);
}
void Game::render() {
	window.clear(sf::Color::Black);

	if (currentGameState == GameState::Menu) {
		mainMenu->draw(window);
	}
	else if (currentGameState == GameState::Info) {
		infoScreen->draw(window);
	}
	else if (currentGameState == GameState::LevelSelect) {
		renderLevelSelect();
	}
	else if (currentGameState == GameState::FreePlay) {
		for (auto& obj : gameObjs)
			if (dynamic_cast<Floor*>(obj.get())) obj->draw(window);
		for (auto& obj : gameObjs)
			if (!dynamic_cast<Floor*>(obj.get())) obj->draw(window);
		renderHUD();
		// Win overlay
		if (_levelComplete) {
			sf::Text winText(mainFont, "LEVEL COMPLETE!", 100);
			winText.setFillColor(sf::Color::Yellow);
			winText.setOutlineColor(sf::Color::Black);
			winText.setOutlineThickness(4.f);
			sf::FloatRect r = winText.getLocalBounds();
			winText.setOrigin({ r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f });
			winText.setPosition({ 960.f, 490.f });
			window.draw(winText);
		}

		// Game Over overlay
		if (gameOver) {
			window.draw(gameOverText);

			auto drawBtn = [&](sf::FloatRect rect, sf::Color col, const std::string& label) {
				sf::RectangleShape btn(rect.size);
				btn.setPosition(rect.position);
				btn.setFillColor(col);
				btn.setOutlineColor(sf::Color::White);
				btn.setOutlineThickness(2.f);
				window.draw(btn);

				sf::Text t(mainFont, label, 32);
				t.setFillColor(sf::Color::White);
				sf::FloatRect tr = t.getLocalBounds();
				t.setOrigin({ tr.position.x + tr.size.x / 2.f, tr.position.y + tr.size.y / 2.f });
				t.setPosition({ rect.position.x + rect.size.x / 2.f,
								 rect.position.y + rect.size.y / 2.f });
				window.draw(t);
				};

			drawBtn({ {640.f, 630.f}, {260.f, 70.f} }, sf::Color(50, 100, 50), "RETRY");
			drawBtn({ {1020.f,630.f}, {260.f, 70.f} }, sf::Color(100, 40, 40), "MAIN MENU");
		}
	}


	window.display();
}
void Game::update() {
	updateMusic();
	if (currentGameState == GameState::Menu) {
		if (mainMenu) mainMenu->update(window);
		return;
	}
	if (currentGameState == GameState::LevelSelect) return;
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

				if (roll >= 60) { 
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
	checkWinCondition();
	gameObjs.erase(
		std::remove_if(gameObjs.begin(), gameObjs.end(),
			[](const std::unique_ptr<Entity>& obj) {return obj->isDestroyed();}
		), gameObjs.end()
	);
}
void Game::checkWinCondition() {
	if (gameOver) return;                         
	if (currentGameState != GameState::FreePlay) return;

	// Redirect after 3s 
	if (_levelComplete) {
		if (_levelCompleteTimer.getElapsedTime().asSeconds() > 2.0f) {
			_levelComplete = false;
			gameObjs.clear();
			currentGameState = GameState::LevelSelect;
		}
		return;                                    
	}

	// check if any enemies remain
	bool hasEnemy = false;
	for (const auto& obj : gameObjs)
		if (dynamic_cast<Enemy*>(obj.get())) { hasEnemy = true; break; }

	if (!hasEnemy) {
		_levelComplete = true;
		_levelCompleteTimer.restart();

		if (_currentDifficulty == Difficulty::Hard) {
			_levelBeatenHard[_currentLevel] = true;
			if (_currentLevel + 1 < 3)
				_levelUnlocked[_currentLevel + 1] = true;

			// Secret level — unlock when all 3 beaten on hard
			if (_levelBeatenHard[0] && _levelBeatenHard[1] && _levelBeatenHard[2])
				_levelUnlocked[3] = true;
		}

		saveProgress();  
	}
}

void Game::loadLevel(int levelIndex, Difficulty diff) {
	static const std::array<std::vector<std::string>, 4> maps = { {
			// Level 1 - the basics
			{
				"#################",
				"#P  XXX X XXX XX#",
				"# #X#X#X#X#X#X# #",
				"# X XXX X XXX X #",
				"#X#X###X###X###X#",
				"#X#X#XXXXXXXXX#X#",
				"#X#XXXXXXXXXXX#X#",
				"#X#X###X#######X#",
				"#X# X#X X XXX #X#",
				"# #XXXX##X#X#X# #",
				"#X  XX# X XXX  E#",
				"#################",
			},
			// Level 2 - getting there
			{
				"#################",
"#P XX#XXXXXXX# E#",
"# #XXXX#####XX ##",
"#X#### XXXX XX#X#",
"#X##XX#####X##XX#",
"#XXXXXXXXXXXXXXX#",
"########X########",
"#XXXXXXXXXXXXXXX#",
"#X######X##### X#",
"# #XXXXXXXXXXXX##",
"# #XXXX#####XX ##",
"#X#### XXXX XX#X#",
"#    XXXXX# #XXX#",
"#E XX##XXX#X#XXX#",
"#################"
			},
			// Level 3 - the true test of skill
			{
				"#################",
				"#PXXXXXXXXXXXXXXX#",  
				"# #X#X#X#X#X#X# #",
				"#XXXXXXXXXXXXXXX#",
				"#X#X#X#X#E#X#X#X#",
				"#XXXXXXXXXXXXXXX#",
				"#X#X#X#E#X#X#X#X#",
				"#XXXXXXXXXXXXXXX#",
				"# #X#X#X#X#X#X# #",
				"#E XXXXXXXXXXX E#",
				"#################",
			},
			// Level 4 - 4/3?
			{
				"#################",
				"#PXXXXXXXXXXXXXXX#",
				"# #X#X#X#X#X#X# #",
				"#XXXXXXXXXXXXXXX#",
				"#X#X#X#X#E#X#X#X#",
				"#XXXXXXXXXXXXXXX#",
				"#X#X#X#E#X#X#X#X#",
				"#XXXXXXXXXXXXXXX#",
				"# #X#X#X#X#X#X# #",
				"#E XXXXXXXXXXX E#",
				"#################",
			}
		} };

	const auto& map = maps[levelIndex];
	float tileSize = 64.0f;

	for (int y = 0; y < (int)map.size(); ++y) {
		for (int x = 0; x < (int)map[y].size(); ++x) {
			float posX = x * tileSize, posY = y * tileSize;
			gameObjs.push_back(std::make_unique<Floor>(posX, posY, texHandler.get("floor")));

			char c = map[y][x];
			if (c == '#') gameObjs.push_back(std::make_unique<Wall>(posX, posY, texHandler.get("wall")));
			if (c == 'X') gameObjs.push_back(std::make_unique<Crate>(posX, posY, texHandler.get("crate")));
			if (c == 'P') {
				auto player = std::make_unique<Player>(
					posX, posY,
					texHandler.get("normPlayer"), texHandler.get("bomb"),
					texHandler.get("explosion"), texHandler.get("cursedPlayer"),
					explosionSound);
				if (diff == Difficulty::Hard) {
					player->setCanKick(true);
					player->setHasLaser(true);
					player->setCanThrow(true);
				}
				gameObjs.push_back(std::move(player));
			
			} 
			if (c == 'E') {
				auto enemy = std::make_unique<Enemy>(
					posX, posY,
					texHandler.get("enemy"), texHandler.get("bomb"),
					texHandler.get("explosion"), 5.f, explosionSound);
				enemy->setDifficulty(diff);  
				gameObjs.push_back(std::move(enemy));
			}
		}
	}
}
void Game::trigerCurse() {
	if (curseTrig) return;
	curseTrig = true;
	gameOver = true;
	
	deathSound.setVolume(50.f);
	deathSound.play();
	
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
	_levelComplete = false;
	deathSound.stop();
	gameObjs.clear();
	currentGameState = GameState::LevelSelect;  
}

void Game::renderHUD() {
	const float hudX = 17 * 64.f + 20.f;  // 
	const float barW = 180.f;
	const float barH = 22.f;
	const float padding = 54.f;
	float curY = 60.f;

	auto drawHpBar = [&](const std::string& label, int hp, int maxHp, sf::Color col) {
		
		sf::Text txt(mainFont, label, 20);
		txt.setFillColor(sf::Color::White);
		txt.setPosition({ hudX, curY });
		window.draw(txt);
		curY += 26.f;

		sf::RectangleShape bg({ barW, barH });
		bg.setPosition({ hudX, curY });
		bg.setFillColor(sf::Color(60, 60, 60));
		bg.setOutlineColor(sf::Color::White);
		bg.setOutlineThickness(1.f);
		window.draw(bg);

		
		float ratio = (maxHp > 0) ? std::clamp((float)hp / maxHp, 0.f, 1.f) : 0.f;
		sf::RectangleShape bar({ barW * ratio, barH });
		bar.setPosition({ hudX, curY });
		bar.setFillColor(col);
		window.draw(bar);

		// Liczba
		sf::Text num(mainFont, std::to_string(hp) + "/" + std::to_string(maxHp), 16);
		num.setFillColor(sf::Color::White);
		num.setPosition({ hudX + barW + 8.f, curY + 2.f });
		window.draw(num);

		curY += padding;
		};

	// Header
	sf::Text header(mainFont, "-- HUD --", 24);
	header.setFillColor(sf::Color(220, 220, 100));
	header.setPosition({ hudX, 20.f });
	window.draw(header);

	// Player
	for (const auto& obj : gameObjs) {
		if (Player* p = dynamic_cast<Player*>(obj.get())) {
			drawHpBar("PLAYER HP", p->getHp(), p->getMaxHp(), sf::Color(50, 200, 80));

			// Bomby
			sf::Text bombInfo(mainFont, "BOMBS: " + std::to_string(p->getMaxBombs()), 20);
			bombInfo.setFillColor(sf::Color::White);
			bombInfo.setPosition({ hudX, curY });
			window.draw(bombInfo);
			curY += 30.f;

			// Zasięg
			sf::Text rangeInfo(mainFont, "RANGE: " + std::to_string(p->getBombRange()), 20);
			rangeInfo.setFillColor(sf::Color(255, 180, 80));
			rangeInfo.setPosition({ hudX, curY });
			window.draw(rangeInfo);
			curY += 44.f;

			break;
		}
	}

	//line separator
	sf::RectangleShape sep({ barW + 60.f, 2.f });
	sep.setPosition({ hudX, curY - 14.f });
	sep.setFillColor(sf::Color(120, 120, 120));
	window.draw(sep);

	// Enemy
	int idx = 1;
	for (const auto& obj : gameObjs) {
		if (Enemy* e = dynamic_cast<Enemy*>(obj.get())) {
			drawHpBar("ENEMY " + std::to_string(idx++),
				e->getHealth(), 3,
				sf::Color(220, 60, 60));
		}
	}
}
void Game::saveProgress() {
	std::ofstream f("save.dat");
	if (!f) return;
	for (bool b : _levelUnlocked)   f << b << " ";
	f << "\n";
	for (bool b : _levelBeatenHard) f << b << " ";
	f << "\n";
}
void Game::loadProgress() {
	std::ifstream f("save.dat"); //no save file == new game;
	if (!f) return;  
	for (bool& b : _levelUnlocked)   f >> b;
	for (bool& b : _levelBeatenHard) f >> b;
}