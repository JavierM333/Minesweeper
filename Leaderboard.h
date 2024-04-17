#include <map>
#include <SFML/Graphics.hpp>

#ifndef TEMP_LEADERBOARD_H
#define TEMP_LEADERBOARD_H
//todo: fill in the leaderboard file

class Leaderboard {
private:
    std::map<int,std::string> scores;
public:
    void fillScores();
};


#endif //TEMP_LEADERBOARD_H
