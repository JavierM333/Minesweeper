#include "Welcome.h"

// Function that helps set the text in the center of the screen
void Welcome::setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

// Function that helps make the text for the Welcome screen
void Welcome::MakeText(sf::Text &text, const sf::Font &font, int size, sf::Color color, const std::string &str, float x,
                       float y) {
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setString(str);
    setText(text, x, y);
}

Welcome::Welcome(int row, int col) {
    Width = col * 32;
    Height = (row * 32) + 100;
    sf::RenderWindow Welcome1(sf::VideoMode(Width, Height), "Welcome", sf::Style::Close | sf::Style::Titlebar);
    // set font
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    // Initialize the text
    sf::Text text;
    MakeText(text, font, 24, sf::Color::White, "Welcome to Minesweeper!", Width / 2, Height / 2 - 150);
    //setting style for the text
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    sf::Text Instructions;
    MakeText(Instructions, font, 20, sf::Color::White, "Enter your name:", Width / 2, Height / 2 - 75);
    Instructions.setStyle(sf::Text::Bold);
    sf::Text Name;
    MakeText(Name, font, 18, sf::Color::Yellow, "", Width / 2, Height / 2 - 45);
    Name.setStyle(sf::Text::Bold);
    while (Welcome1.isOpen()) {
        sf::Event event{};
        // handle events for the window
        while (Welcome1.pollEvent(event)) {
            switch (event.type) {
                // If the window is closed manually, shut down the whole program
                case sf::Event::Closed:
                    Welcome1.close();
                    closed = true;
                    break;
                    // handle text entered
                case sf::Event::TextEntered:
                    if (event.text.unicode < 128 && event.text.unicode != 8) {
                        char enteredChar = static_cast<char>(event.text.unicode);
                        if (isalpha(enteredChar)) { // check if the entered character is a letter
                            if (name.size() > 10) {
                                break;
                            }
                            if (name.size() == 0) {
                                name += std::toupper(enteredChar);
                            } else {
                                name += std::tolower(enteredChar);
                            }
                            Name.setString(name + "|");
                            setText(Name, Width / 2, Height / 2 - 45);
                        }
                    }
                    break;
                    // handle backspace
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::BackSpace) {
                        if (!name.empty()) {
                            name.pop_back();
                            Name.setString(name + "|");
                            setText(Name, Width / 2, Height / 2 - 45);
                        }
                    }
                    if (event.key.code == sf::Keyboard::Return) {
                        if (!name.empty()) {
                            Welcome1.close();
                        }
                    }
                    break;
            }
        }
        Welcome1.clear(sf::Color::Blue);
        Welcome1.draw(text);
        Welcome1.draw(Name);
        Welcome1.draw(Instructions);
        Welcome1.display();
    }
}

bool Welcome::isClosed() const { return closed; }
