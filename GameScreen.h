#include "SFML/Graphics.hpp"
#include "Leaderboard.h"
#include <vector>

#ifndef MINESWEEPER_GAMESCREEN_H
#define MINESWEEPER_GAMESCREEN_H

//button struct
struct Button {
    sf::Texture texture;
    sf::Sprite sprite;
    std::function<void()> onClick;
};

// Tile struct
struct Tile {
    bool mine;
    bool flag;
    bool revealed;
    int neighborMines;
    // array of all neighbor tiles
    Tile *neighbors[8];
    sf::Texture texture;
    sf::Texture bottom;
};

class GameScreen {
private:
    std::string name;
    int row;
    int col;
    int count;
    int mineCount;
    int total;
    bool RestartStatus = false;
    bool leaderboardstatus = false;
    bool DebugMode = false;
    bool Running = true;
    bool won = false;
    int paused = 0;
    //texture for the timer, counter
    sf::Texture digits;
    sf::Texture revealedTexture;
    sf::Texture hiddenTexture;
    sf::Texture flagTexture;
    sf::Texture mineTexture;
    sf::Texture winTexture;
    sf::Texture loseTexture;
    sf::Texture happyTexture;
    sf::Texture debugTexture;
    std::chrono::time_point<std::chrono::system_clock> start;
    // pointers of buttons and tiles
    std::vector<std::unique_ptr<Tile>> tiles;
    std::unique_ptr<Button> FaceButton;
    std::vector<std::unique_ptr<Button>> Counter;
    std::unique_ptr<Button> leaderboard;
    std::unique_ptr<Button> Debug;
    std::unique_ptr<Button> PausePlay;
    std::vector<std::unique_ptr<Button>> timer;

    void CounterUpdate(int i);

    void TimerUpdate();

    void Validate();

    void addToLeaderboard();

    void displayTiles(GameScreen &game);

    void Render(sf::RenderWindow &Game);

    void updateTileTexture(bool revealAll);

    void HandleMines(Tile *tile);

    void HandleNotMines(Tile *tile);

    void handleFlags(Tile *tile);

public:
    GameScreen(int row, int col, int mines, std::string string);

    bool getRestartStatus() const;
};


#endif //MINESWEEPER_GAMESCREEN_H
