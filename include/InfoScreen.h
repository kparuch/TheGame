#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class InfoScreen {
public:
    explicit InfoScreen(sf::Font& font);

    /**
     * @param icons  mapa: klucz = nazwa pickupu (np. "Tinfoil Cap"),
     *               wartość = wskaźnik do załadowanej textury.
     *               Jeśli dana nazwa nie istnieje lub ptr == nullptr,
     *               rysuje szary placeholder z "?".
     *               Klucze muszą zgadzać się z nazwami w tablicy pickups
     *               zdefiniowanej w InfoScreen.cpp.
     */
    void draw(sf::RenderWindow& window,
        const std::unordered_map<std::string, const sf::Texture*>& icons);

    /** Przekaż tu zdarzenia okna — obsługuje MouseWheelScrolled. */
    void handleEvent(const sf::Event& event);

    /** Resetuje scroll do góry — wywołaj przy każdym otwarciu ekranu. */
    void resetScroll();

private:
    sf::Font& font;
    float     scrollOffset = 0.f;

    static constexpr float ICON_SIZE = 64.f;   // px, ikona pickupu
    static constexpr float SCROLL_SPEED = 32.f;   // px na jeden tick kółka
    static constexpr float MAX_SCROLL = 320.f;  // dostosuj jeśli dodasz więcej treści
};
