#include "InfoScreen.h"
#include <algorithm>
#include <array>
#include <cmath>

InfoScreen::InfoScreen(sf::Font& font) : font(font) {}

void InfoScreen::resetScroll() { scrollOffset = 0.f; }

void InfoScreen::handleEvent(const sf::Event& event) {
    if (const auto* w = event.getIf<sf::Event::MouseWheelScrolled>()) {
        scrollOffset -= w->delta * SCROLL_SPEED;
        scrollOffset = std::clamp(scrollOffset, 0.f, MAX_SCROLL);
    }
}

void InfoScreen::draw(sf::RenderWindow& window,
    const std::unordered_map<std::string, const sf::Texture*>& icons)
{
    const sf::Vector2f wSize(window.getSize());

    // ── colour ──────────────────────────────────────────────────────────────
    const sf::Color ink(35, 20, 5);
    const sf::Color header(90, 45, 10);
    const sf::Color faded(110, 90, 60);
    const sf::Color red(160, 30, 30);
    const sf::Color purple(100, 0, 130);
    const sf::Color sepCol(140, 110, 60, 150);

    // ── helpers  ────────────────────────────────
    auto drawRect = [&](float x, float y, float w, float h, sf::Color fill,
        sf::Color outline = sf::Color::Transparent, float thick = 0.f) {
            sf::RectangleShape r({ w, h });
            r.setPosition({ x, y });
            r.setFillColor(fill);
            if (thick > 0.f) { r.setOutlineColor(outline); r.setOutlineThickness(thick); }
            window.draw(r);
        };

    auto drawLine = [&](float x, float y, float w, sf::Color c) {
        sf::RectangleShape l({ w, 1.f });
        l.setPosition({ x, y });
        l.setFillColor(c);
        window.draw(l);
        };

    auto drawText = [&](const std::string& s, float x, float y,
        unsigned sz, sf::Color c) {
            sf::Text t(font, s, sz);
            t.setFillColor(c);
            t.setPosition({ x, y });
            window.draw(t);
        };

    auto drawTextCentered = [&](const std::string& s, float y,
        unsigned sz, sf::Color c) {
            sf::Text t(font, s, sz);
            t.setFillColor(c);
            sf::FloatRect r = t.getLocalBounds();
            t.setOrigin({ r.position.x + r.size.x / 2.f, 0.f });
            t.setPosition({ 960.f, y });
            window.draw(t);
        };


    auto drawIcon = [&](const std::string& name, float x, float y) {
        auto it = icons.find(name);
        if (it != icons.end() && it->second) {
            sf::Sprite sp(*it->second);
        
            auto ts = it->second->getSize();
            unsigned frameW = ts.x / 2;   // 
            unsigned frameH = ts.y / 2;        //
            sp.setTextureRect(sf::IntRect({ 0, 0 }, { (int)frameW, (int)frameH }));

            //scale
            float sc = ICON_SIZE / static_cast<float>(std::max(frameW, frameH));
            sp.setScale({ sc, sc });
            sp.setPosition({ x, y });
            window.draw(sp);
        }
        else {
            // placeholder — szare pole z "?"
            drawRect(x, y, ICON_SIZE, ICON_SIZE,
                sf::Color(55, 45, 28),
                sf::Color(110, 88, 52), 1.f);
            sf::Text q(font, "?", 20);
            q.setFillColor(sf::Color(140, 120, 80));
            sf::FloatRect qr = q.getLocalBounds();
            q.setOrigin({ qr.position.x + qr.size.x / 2.f,
                          qr.position.y + qr.size.y / 2.f });
            q.setPosition({ x + ICON_SIZE / 2.f, y + ICON_SIZE / 2.f });
            window.draw(q);
        }
        };


    window.clear(sf::Color(10, 8, 5));

    // monitor
    drawRect(50.f, 50.f, 1820.f, 980.f,
        sf::Color(18, 15, 10), sf::Color(60, 50, 30), 4.f);
    // papier
    drawRect(110.f, 90.f, 1700.f, 900.f, sf::Color(232, 220, 185));
    // linie pomocnicze
    for (int i = 0; i < 30; ++i)
        drawRect(110.f, 122.f + i * 30.f, 1700.f, 1.f, sf::Color(180, 165, 130, 80));
    // margines
    drawRect(210.f, 90.f, 2.f, 900.f, sf::Color(180, 60, 60, 160));

    // ── tytuł (stały, nie scrolluje się) ────────────────────────────────────
    {
        sf::Text title(font, "SOYMAN II: THE WAY OF THE BOMB", 48);
        title.setFillColor(sf::Color(40, 25, 10));
        title.setOutlineColor(sf::Color(180, 150, 90));
        title.setOutlineThickness(1.f);
        sf::FloatRect tr = title.getLocalBounds();
        title.setOrigin({ tr.position.x + tr.size.x / 2.f, 0.f });
        title.setPosition({ 960.f, 100.f });
        window.draw(title);
    }
    drawLine(260.f, 168.f, 1400.f, sf::Color(100, 70, 30));


    constexpr float VP_LEFT = 110.f;
    constexpr float VP_TOP = 172.f;
    constexpr float VP_W = 1700.f;
    constexpr float VP_H = 768.f;   // 940 - 172

    sf::View scrollView;
    scrollView.setViewport(sf::FloatRect(
        { VP_LEFT / wSize.x,  VP_TOP / wSize.y },
        { VP_W / wSize.x,  VP_H / wSize.y }
    ));
    // Środek view przesuwa się o scrollOffset → efekt scrollowania.
    scrollView.setSize({ VP_W, VP_H });
    scrollView.setCenter({ VP_LEFT + VP_W / 2.f,
                           VP_TOP + VP_H / 2.f + scrollOffset });
    window.setView(scrollView);

    // ── LORE ────────────────────────────────────────────────────────────────
    drawText("[ LORE ]", 270.f, 182.f, 28, header);
    drawText("The Silent one, listen to me, truly I tell you, the great danger is ahead",
        230.f, 222.f, 21, ink);
    drawText("I am pretty sure, that basics are known to you, I hope you still remember the training of yours",
        230.f, 252.f, 21, ink);
    drawText("WASD, the movement is classic, bomg gives you SPACE, the things you will find down there, have different variations...",
        230.f, 282.f, 21, ink);
    drawText("Beware, if you perish, so will the world around you, the environment here is cursed...",
        230.f, 312.f, 21, red);
    drawLine(230.f, 352.f, 1400.f, sepCol);

    // ── PICKUPS ─────────────────────────────────────────────────────────────
    drawText("[ PICKUPS ]", 230.f, 362.f, 28, header);

    struct PickupInfo { std::string name, desc; sf::Color nameCol; };
    const std::array<PickupInfo, 8> pickups = { {
        { "Tinfoil",    "Signal booster  increases the range",                          ink    },
        { "Raw Meat",       "Can be a life saver, may increase HP by 2",                     ink    },
        { "Chocolate Milk", "ALWAYS PICK THIS UP SILENT ONE",                                faded  },
        { "Soy Milk",       "silent one, i do hope your love for this drink is long gone",   ink    },
        { "Pigeon",         "A noteworthy companion, ",                   ink    },
        { "Bomb Up",        "As the name suggests...",                                       ink    },
        { "Raw Meat ?",            "if you stop, you perish..., 1 ...",                                                    red    },
        { "???",       "shrouded in mystery, its function: De.DOS_ERROR",      purple },
    } };

	constexpr float ROW_H = 70.f;   //height of one pickup row (icon + text)
    constexpr float PU_START = 408.f;

    for (int i = 0; i < (int)pickups.size(); ++i) {
        const float py = PU_START + i * ROW_H;

        
        drawIcon(pickups[i].name, 240.f, py + (ROW_H - ICON_SIZE) / 2.f);

        // pickup name
        {
            sf::Text nt(font, pickups[i].name, 21);
            nt.setFillColor(pickups[i].nameCol);
            nt.setPosition({ 320.f, py + 4.f });
            window.draw(nt);
        }
		// description
        drawText(pickups[i].desc, 320.f, py + 27.f, 18, faded);
    }

    
    const float afterPU = PU_START + (int)pickups.size() * ROW_H + 10.f;
    drawLine(230.f, afterPU, 1400.f, sepCol);

    // ── TIPS ────────────────────────────────────────────────────────────────
    const float tipsY = afterPU + 14.f;
    drawText("[ TIPS ]", 230.f, tipsY, 28, header);
    drawText("-  Crates have a 30% chance to drop, so destroy everything you see.",
        230.f, tipsY + 42.f, 21, ink);
    drawText("-  Enemies on HARD and NIGHTMARE mode behave differently, so be cautious.",
        230.f, tipsY + 78.f, 21, ink);
    drawText("-  Your bombs will also damage you.",
        230.f, tipsY + 114.f, 21, red);

    // ── secret area ────────────────────────────────────────────────────────
    const float secretY = tipsY + 158.f;
    drawRect(228.f, secretY, 1400.f, 92.f,
        sf::Color(60, 0, 80, 60),
        sf::Color(150, 0, 200, 120), 1.f);
    drawText("?  Apparently, there are four levels, yet you will ever only see 3, as the level you seek is not here",
        238.f, secretY + 12.f, 21, purple);
    drawText("?  As it was foretold, four pickups were made to alter the game, you will only see here two...",
        238.f, secretY + 44.f, 21, purple);

 
    window.setView(window.getDefaultView());

    // ── scrollbar ────────────────────────────────────────────────────────────
    
    if (MAX_SCROLL > 0.f) {
        constexpr float SB_X = 1806.f;
        constexpr float SB_TOP = VP_TOP;
        constexpr float SB_H = VP_H;

        drawRect(SB_X, SB_TOP, 5.f, SB_H, sf::Color(140, 120, 80, 55));

        const float thumbH = SB_H * (SB_H / (SB_H + MAX_SCROLL));
        const float thumbY = SB_TOP + (scrollOffset / MAX_SCROLL) * (SB_H - thumbH);
        drawRect(SB_X, thumbY, 5.f, thumbH, sf::Color(130, 100, 50, 190));
    }


    drawRect(110.f, 90.f, 1700.f, VP_TOP - 90.f, sf::Color(232, 220, 185));

 
    {
        sf::Text title(font, "SOYMAN II: THE WAY OF THE BOMB", 48);
        title.setFillColor(sf::Color(40, 25, 10));
        title.setOutlineColor(sf::Color(180, 150, 90));
        title.setOutlineThickness(1.f);
        sf::FloatRect tr = title.getLocalBounds();
        title.setOrigin({ tr.position.x + tr.size.x / 2.f, 0.f });
        title.setPosition({ 960.f, 100.f });
        window.draw(title);
    }
    drawLine(260.f, 168.f, 1400.f, sf::Color(100, 70, 30));

    drawRect(110.f, 940.f, 1700.f, 50.f, sf::Color(232, 220, 185));
//footer    
    drawLine(110.f, 940.f, 1700.f, sf::Color(100, 70, 30));
    drawTextCentered("scroll with mouse wheel click anywhere to return to main menu",
        952.f, 20, faded);
}
