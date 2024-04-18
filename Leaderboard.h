#include <map>
#include <SFML/Graphics.hpp>

#ifndef TEMP_LEADERBOARD_H
#define TEMP_LEADERBOARD_H

class Leaderboard {
private:
    sf::Font font;
    int width;
    int height;
    std::map<int, std::pair<std::string, std::string>> scores;
    bool isOpen;
    sf::RenderWindow leaderboardWindow;
    std::string finalLeaderboard;
public:
    Leaderboard(int row, int col);

    bool IsOpen() const;

    void readScores();

    void fillScores();

    static int timeToSeconds(const std::string &time) {
        int colonIndex(time.find(':'));
        int minutes = std::stoi(time.substr(0, colonIndex));
        int seconds = std::stoi(time.substr(3, 2));
        return minutes * 60 + seconds;
    }
    bool isWindowOpen() const {
        return leaderboardWindow.isOpen();
    }
};


#endif //TEMP_LEADERBOARD_H
