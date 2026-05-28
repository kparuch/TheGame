#include "InfoScreen.h"

InfoScreen::InfoScreen(sf::Font& font) : font(font) {}

void InfoScreen::draw(sf::RenderWindow& window) {

    window.clear(sf::Color(10, 8, 5));

    sf::RectangleShape monitor({ 1820.f, 980.f });
    monitor.setPosition({ 50.f, 50.f });
    monitor.setFillColor(sf::Color(18, 15, 10));
    monitor.setOutlineColor(sf::Color(60, 50, 30));
    monitor.setOutlineThickness(4.f);
    window.draw(monitor);

    sf::RectangleShape paper({ 1700.f, 900.f });
    paper.setPosition({ 110.f, 90.f });
    paper.setFillColor(sf::Color(232, 220, 185));
    window.draw(paper);

    for (int i = 0; i < 28; ++i) {
        sf::RectangleShape line({ 1700.f, 1.f });
        line.setPosition({ 110.f, 122.f + i * 30.f });
        line.setFillColor(sf::Color(180, 165, 130, 80));
        window.draw(line);
    }

    sf::RectangleShape margin({ 2.f, 900.f });
    margin.setPosition({ 210.f, 90.f });
    margin.setFillColor(sf::Color(180, 60, 60, 160));
    window.draw(margin);

    // --- helpers ---
    auto drawLine = [&](float x, float y, float w, sf::Color col) {
        sf::RectangleShape l({ w, 1.f });
        l.setPosition({ x, y });
        l.setFillColor(col);
        window.draw(l);
        };

    auto drawText = [&](const std::string& str, float x, float y,
        unsigned size, sf::Color col) {
            sf::Text t(font, str, size);
            t.setFillColor(col);
            t.setPosition({ x, y });
            window.draw(t);
        };

    auto drawTextCentered = [&](const std::string& str, float y,
        unsigned size, sf::Color col) {
            sf::Text t(font, str, size);
            t.setFillColor(col);
            sf::FloatRect r = t.getLocalBounds();
            t.setOrigin({ r.position.x + r.size.x / 2.f, 0.f });
            t.setPosition({ 960.f, y });
            window.draw(t);
        };

    const sf::Color ink(35, 20, 5);
    const sf::Color header(90, 45, 10);
    const sf::Color faded(110, 90, 60);
    const sf::Color red(160, 30, 30);
    const sf::Color purple(100, 0, 130);

    
    sf::Text title(font, "SOYMAN II: THE WAY OF THE BOMB", 48);
    title.setFillColor(sf::Color(40, 25, 10));
    title.setOutlineColor(sf::Color(180, 150, 90));
    title.setOutlineThickness(1.f);
    sf::FloatRect tr = title.getLocalBounds();
    title.setOrigin({ tr.position.x + tr.size.x / 2.f, 0.f });
    title.setPosition({ 960.f, 100.f });
    window.draw(title);
    drawLine(260.f, 165.f, 1400.f, sf::Color(100, 70, 30));

    // --- LORE ---
    drawText("[ LORE ]", 230.f, 180.f, 28, header);
    drawText("The Silent one, listen to me, truly I tell you, the great danger is ahead ", 230.f, 218.f, 21, ink);
    drawText("I am pretty sure, that basics are known to you, I hope you still remember the training of yours", 230.f, 248.f, 21, ink);
    drawText("WASD, the movement is classic, bomg gives you SPACE, the things you will find down there, have different variations...", 230.f, 278.f, 21, ink);
    drawText("Beware, if you perish, so will the world around you, the environment here is cursed...", 230.f, 308.f, 21, red);
    drawLine(230.f, 345.f, 1400.f, sf::Color(140, 110, 60, 150));

    // --- PICKUPY ---
    drawText("[ PICKUPS ]", 230.f, 325.f, 28, header);

    struct PickupInfo { std::string name, desc; sf::Color col; };
    const std::array<PickupInfo, 8> pickups = { {
        { "Tinfoil Cap",    "Signal booster, increases the range",               ink   },
        { "Raw Meat",       "Can be a live saver, may increase HP by 2",       ink   },
        { "Chocolate Milk", "ALWAYS PICK THIS UP SILENT ONE",     faded },
        { "Soy Milk",       "silent one, i do hope your love for this drink is long gone",      ink   },
        { "Pigeon",         "A noteworthy companion, he will train you, you will be faster",                ink   },
        { "Bomb Up",        "As the name suggests...",                       ink   },
        { "???",            "[REDACTED]",         red   },
        { "IAMERROR",            "These are not all the pick ups, other ones have been lost for ages..",         purple   },
    } };

    for (int i = 0; i < (int)pickups.size(); ++i) {
        float py = 365.f + i * 38.f;
        sf::CircleShape bullet(5.f);
        bullet.setFillColor(sf::Color(100, 70, 30));
        bullet.setPosition({ 240.f, py + 7.f });
        window.draw(bullet);
        drawText(pickups[i].name + ": " + pickups[i].desc, 260.f, py, 21, pickups[i].col);
    }
    drawLine(230.f, 640.f, 1400.f, sf::Color(140, 110, 60, 150));

    // --- TIPS ---
    drawText("[ TIPS ]", 230.f, 650.f, 28, header);
    drawText("-  Crates have a 30% chance to drop, so destroy everything you see.", 230.f, 690.f, 21, ink);
    drawText("-  Enemies on HARD behave differently, so be cautious.", 230.f, 725.f, 21, ink);
    drawText("-  Your bombs will also damage you", 230.f, 760.f, 21, red);

    sf::RectangleShape secretBox({ 1400.f, 88.f });
    secretBox.setPosition({ 228.f, 798.f });
    secretBox.setFillColor(sf::Color(60, 0, 80, 60));
    secretBox.setOutlineColor(sf::Color(150, 0, 200, 120));
    secretBox.setOutlineThickness(1.f);
    window.draw(secretBox);
    drawText("?  Apparently, there are four levels, yet you will only see 3, the level you seek will not be found here",
        238.f, 808.f, 21, purple);
	drawText("?  More pickups are yet to be found...", 238.f, 838.f, 21, purple);

    // --- FOOTER ---
    drawLine(110.f, 940.f, 1700.f, sf::Color(100, 70, 30));
    drawTextCentered("click anywhere to return to main menu...", 952.f, 20, faded);
}