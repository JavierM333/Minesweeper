#ifndef MINESWEEPER_Welcome1_H
#define MINESWEEPER_Welcome1_H

#include "SFML/Graphics.hpp"
#include <string>

class Welcome {
private:
    std::string name;
    int Width;
    int Height;
    bool closed = false;
public:
    Welcome(int row, int col);

    std::string getName() { return name;};

    bool isClosed() const;

    static void
    MakeText(sf::Text &text, const sf::Font &font, int size, sf::Color color, const std::string &str, float x, float y);

    static void setText(sf::Text &text, float x, float y);
};

#endif //MINESWEEPER_Welcome1_H