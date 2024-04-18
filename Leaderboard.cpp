#include <SFML/Graphics.hpp>
#include <map>
#include <fstream>
#include "Leaderboard.h"

Leaderboard::Leaderboard(int row, int col) : width(col * 16), height((row * 16) + 50), leaderboardWindow(sf::VideoMode(width, height), "Leaderboard") {
    isOpen = true;
    readScores();
    fillScores();
    font.loadFromFile("files/font.ttf");
    // LEADERBOARD TEXT
    sf::Text leader;
    leader.setFont(font);
    leader.setString("LEADERBOARD");
    leader.setCharacterSize(20);
    leader.setFillColor(sf::Color::White);
    leader.setStyle(sf::Text::Bold | sf::Text::Underlined);
    leader.setOrigin(leader.getLocalBounds().width / 2, leader.getLocalBounds().height / 2);
    leader.setPosition(width / 2, (height / 2) - 120);
    // TEXT FOR LEADERBOARD
    sf::Text text;
    text.setFont(font);
    text.setString(finalLeaderboard);
    text.setCharacterSize(18);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::White);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(width / 2.0f, (height / 2.0f) + 20));
    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
            }
        }
        leaderboardWindow.clear(sf::Color::Blue);
        leaderboardWindow.draw(text);
        leaderboardWindow.draw(leader);
        leaderboardWindow.display();
    }
    isOpen = false;
}

bool Leaderboard::IsOpen() const {
    return isOpen;
}

void Leaderboard::readScores() {
    std::ifstream file("files/leaderboard.txt", std::ios::in);
    if (!file.is_open()) {
        printf("Error: leaderboard.txt not found!\n");
        return;
    }
    std::string score;
    std::string name;
    while (getline(file, score, ',')) {
        getline(file, name);
        printf("%s\n", score.c_str());
        scores[timeToSeconds(score)] = {score, name};
    }
    file.close();
}

void Leaderboard::fillScores() {
    int i = 1;
    finalLeaderboard.clear();
    for (auto it = scores.begin(); it != scores.end(); it++) {
        finalLeaderboard += std::to_string(i) + ". " + "\t" + it->second.first + "\t" + it->second.second + "\n\n";
        i++;
    }
}


