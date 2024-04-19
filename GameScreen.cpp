#include <string>
#include <fstream>
#include <sstream>
#include "GameScreen.h"
#include "Leaderboard.h"
#include "SFML/Graphics.hpp"
#include <ctime>

using namespace std;

GameScreen::GameScreen(int row, int col, int mines, std::string name) : row(row), col(col), name(name) {
    count = mines;
    mineCount = mines;
    int mineCount = mines;
    total = row * col - mines;
    start = std::chrono::system_clock::now();
    srand(time(nullptr)); // Seed the random number generator
    double mineRatio = static_cast<double>(mines) / (row * col);
    // Texture for mines
    mineTexture.loadFromFile("files/images/mine.png");
    // Texture for hidden tiles
    hiddenTexture.loadFromFile("files/images/tile_hidden.png");
    // Texture for revealed
    revealedTexture.loadFromFile("files/images/tile_revealed.png");
    // Texture for flags
    flagTexture.loadFromFile("files/images/flag.png");
    // Texture for win face
    winTexture.loadFromFile("files/images/face_win.png");
    // Texture for lose face
    loseTexture.loadFromFile("files/images/face_lose.png");
    // Texture for happy face
    happyTexture.loadFromFile("files/images/face_happy.png");
    for (int i = 0; i < row * col; i++) {
        auto tile = std::make_unique<Tile>();
        tile->texture = hiddenTexture;
        if (static_cast<double>(rand()) / RAND_MAX < mineRatio && mines > 0) {
            tile->mine = true;
            mines--;
        }
        tiles.push_back(std::move(tile));  // Move the tile with the set texture into the vector
    }
    while (mines > 0) {
        tiles[rand() % (row * col)]->mine = true;
        mines--;
    }
    for (int i = 0; i < row * col; i++) {
        Tile *tile = tiles[i].get();
        int x = i % col;
        int y = i / col;

        // Initialize all neighbors to nullptr
        std::fill(std::begin(tile->neighbors), std::end(tile->neighbors), nullptr);

        // Assign neighbors if they exist
        if (x > 0) tile->neighbors[0] = tiles[i - 1].get();
        if (x < col - 1) tile->neighbors[3] = tiles[i + 1].get();
        if (y > 0) tile->neighbors[6] = tiles[i - col].get();
        if (y < row - 1) tile->neighbors[7] = tiles[i + col].get();
        if (x > 0 && y > 0) tile->neighbors[1] = tiles[i - col - 1].get();
        if (x > 0 && y < row - 1) tile->neighbors[2] = tiles[i + col - 1].get();
        if (x < col - 1 && y > 0) tile->neighbors[4] = tiles[i - col + 1].get();
        if (x < col - 1 && y < row - 1) tile->neighbors[5] = tiles[i + col + 1].get();
    }
    // happy face
    FaceButton = std::make_unique<Button>();
    FaceButton->texture = happyTexture;
    FaceButton->sprite.setTexture(FaceButton->texture);
    FaceButton->sprite.setPosition(((col / 2) * 32) - 32, 32 * (row + 0.5));
    FaceButton->onClick = [this]() {
        this->RestartStatus = true;
    };
    // Debug button
    Debug = std::make_unique<Button>();
    Debug->texture.loadFromFile("files/images/debug.png");
    Debug->sprite.setTexture(Debug->texture);
    Debug->sprite.setPosition((col * 32) - 304, 32 * (row + 0.5));
    Debug->onClick = [this]() {
        if (won) { return; }
        DebugMode = !DebugMode; // Toggle DebugMode
        for (auto &t: tiles) {
            if (DebugMode) {
                if (!t->mine) continue;
                t->texture = mineTexture;
                t->bottom = hiddenTexture;
            } else {
                // If DebugMode is off, check if the tile is flagged
                if (t->mine && !t->flag) {
                    t->texture = hiddenTexture;
                } else if (t->flag) {
                    t->texture = flagTexture;
                }
            }
        }
    };

    // PlayPause button
    //todo:: pause should reveal the whole board
    PausePlay = std::make_unique<Button>();
    PausePlay->texture.loadFromFile("files/images/play.png");
    PausePlay->sprite.setTexture(PausePlay->texture);
    PausePlay->sprite.setPosition((col * 32) - 240, 32 * (row + 0.5));
    PausePlay->onClick = [this]() {
        if (won) { return; }
        Running = !Running; // Toggle Running
        if (Running) {
            PausePlay->texture.loadFromFile("files/images/play.png");
            start = std::chrono::system_clock::now() - std::chrono::seconds(paused);
        } else {
            PausePlay->texture.loadFromFile("files/images/pause.png");
            paused = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
        }
    };
    //Counter Button
    digits.loadFromFile("files/images/digits.png");
    for (int i = 0; i < 4; ++i) {
        auto digit = std::make_unique<Button>();
        digit->sprite.setTexture(digits);
        digit->sprite.setTextureRect(sf::IntRect(0, 0, 21, 32)); // Set to '0'
        digit->sprite.setPosition(33 + (i * 21), 32 * (row + 0.5) + 16);
        Counter.push_back(std::move(digit));
    }
    CounterUpdate(row);
    // Timer Button
    for (int i = 0; i < 2; i++) {
        auto digit = std::make_unique<Button>();
        digit->sprite.setTexture(digits);
        digit->sprite.setTextureRect(sf::IntRect(0, 0, 21, 32)); // Set to '0'
        digit->sprite.setPosition((col * 32) - 97 + i * 21, 32 * (row + 0.5) + 16); // Minute digits
        timer.push_back(std::move(digit));
    }
    for (int i = 2; i < 4; i++) {
        auto digit = std::make_unique<Button>();
        digit->sprite.setTexture(digits);
        digit->sprite.setTextureRect(sf::IntRect(0, 0, 21, 32)); // Set to '0'
        digit->sprite.setPosition((col * 32) - 97 + i * 21, 32 * (row + 0.5) + 16); // Second digits
        timer.push_back(std::move(digit));
    }
    leaderboard = std::make_unique<Button>();
    leaderboard->texture.loadFromFile("files/images/leaderboard.png");
    leaderboard->sprite.setTexture(leaderboard->texture);
    leaderboard->sprite.setPosition((col * 32) - 176, 32 * (row + 0.5));
    leaderboard->onClick = [this]() {
        if (won) {
            addToLeaderboard();
        }
        Leaderboard board(this->row, this->col);
    };
    /*
     *
     ******************************************************** Render Window Separation **********************************************************************
     *
     */
    sf::RenderWindow Game(sf::VideoMode(col * 32, (row * 32) + 100), "Minesweeper",
                          sf::Style::Close | sf::Style::Titlebar);
    Game.setFramerateLimit(30);
    // runs while the game is open
    while (Game.isOpen()) {
        if (Running) {
            TimerUpdate();
        }
        if (getRestartStatus()) {
            Game.close();
        }
        sf::Event event{};
        // handle events for the window
        while (Game.pollEvent(event)) {
            switch (event.type) {
                default:
                    break;
                    // closes the window
                case sf::Event::Closed:
                    Game.close();
                    break;
                    // handles the mouse click, checking mouse coordinates
                case sf::Event::MouseButtonPressed:
                    sf::Vector2i mousePos = sf::Mouse::getPosition(Game);
                    int tileX = mousePos.x / 32;
                    int tileY = mousePos.y / 32;
                    if (PausePlay->sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if(Running){updateTileTexture(true);} else {updateTileTexture(false);}
                        Render(Game);
                        PausePlay->onClick();
                    }
                    if (FaceButton->sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        FaceButton->onClick();
                    }//leaderboard button pressed
                    if (!Running) { break; }
                    if (leaderboard->sprite.getGlobalBounds().contains(mousePos.x, mousePos.y) && Running) {
                        PausePlay->onClick();
                        leaderboardstatus = true;
                        //todo: even if game is paused, after leaderboard is closed, all actions done when paused shows
                        //todo: pause button works (once) after game is won and leaderboard is closed
                        updateTileTexture(true);
                        PausePlay->onClick();
                    }
                    if (won) {break;}
                    //checks which sprite was clicked
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (tileX >= 0 && tileX < col && tileY >= 0 && tileY < row) {
                            Tile *tile = tiles[tileY * col + tileX].get();
                            if (tile->revealed || tile->flag) { break; }
                            if (tile->flag) {
                                count++;
                                CounterUpdate(row);
                            }
                            if (tile->mine) {
                                // handle when the tile is a mine
                                FaceButton->texture = loseTexture;
                                GameScreen::HandleMines(tile);
                                break;
                            }
                            // handle when the tile is not a mine
                            HandleNotMines(tile);
                            //implement Counter to count the number of tiles that are mines

                        }
                        // handle when the face is clicked
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                            Debug->sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            Debug->onClick();
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if (tileX >= 0 && tileX < col && tileY >= 0 && tileY < row) {
                            Tile *tile = tiles[tileY * col + tileX].get();
                            if (tile->revealed) { break; }
                            if (tile->flag) {
                                tile->flag = false;
                                tile->texture = hiddenTexture;
                                count++;
                            } else {
                                handleFlags(tile);
                            }
                            CounterUpdate(row);
                        }
                    }
                    break;
            }
        }
        Render(Game);
    }
}

/*
 *
 **************************************************************** End of Render Window *********************************************************************
 *
 */

bool GameScreen::getRestartStatus() const {
    return RestartStatus;
}

void GameScreen::HandleMines(Tile *tile) {
    //reveals all mines
    //todo: win face shows when you press a mine instead of loss face
    tile->revealed = true;
    tile->texture = mineTexture;
    for (auto &t: tiles) {
        t->revealed = true;
        if (t->mine) {
            t->texture = mineTexture;
            t->bottom = hiddenTexture;;
        }
    }
    FaceButton->texture = loseTexture; // Set the FaceButton sprite's texture to the lose face texture
    Validate();
}

void GameScreen::HandleNotMines(Tile *tile) {
    //recursively reveals all tiles that are not mines and their neighbors
    if (tile->revealed) { return; }
    // adds to each tile the amount of mines nearby
    for (auto t: tile->neighbors) {
        if (t == nullptr) { continue; }
        if (t->mine) { tile->neighborMines++; }
    }
    tile->revealed = true;
    if (tile->neighborMines > 0) {
        // if there are nearby mines, display the amount of mines nearby
        tile->bottom = revealedTexture;
        std::string filename = "files/images/number_" + std::to_string(tile->neighborMines) + ".png";
        tile->texture.loadFromFile(filename);
    } else {
        // makes all the tiles that are null pointers revealed
        tile->texture = revealedTexture;
        for (auto t: tile->neighbors) {
            if (t == nullptr) { continue; }
            HandleNotMines(t);
        }
    }
    Validate();
}

void GameScreen::CounterUpdate(int i) {
    //updates the count
    //abs count to have only positive value
    int absCount = abs(count);
    // in the case of flags exceeding mines, this will display a negative number
    if (count < 0) {
        if (Counter.size() == 3 || Counter.size() > 4) {
            auto neg = std::make_unique<Button>();
            neg->sprite.setTexture(digits);
            neg->sprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32)); // Set to '-'
            neg->sprite.setPosition(12, 32 * (i + 0.5) + 16);
            Counter.insert(Counter.begin(), std::move(neg));
        }
        while (Counter.size() > 4) {
            Counter.erase(Counter.begin() + 1);
        }
    } else if (count >= 0 && Counter.size() > 3) {
        Counter.erase(Counter.begin());
    }
    for (int i = 0; i < 3; i++) {
        int digit = absCount / static_cast<int>(pow(10, 2 - i));
        if (i + (Counter.size() - 3) < Counter.size()) {
            Counter[i + (Counter.size() - 3)]->sprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32));
        }
        absCount %= static_cast<int>(pow(10, 2 - i));
    }
}

void GameScreen::TimerUpdate() {
    if (won) { return; }
    //updates the timer
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
    int minutes = elapsed.count() / 60;
    int seconds = elapsed.count() % 60;
    for (int i = 0; i < 2; i++) {
        int digit = minutes / static_cast<int>(pow(10, 1 - i));
        timer[i]->sprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32));
        minutes %= static_cast<int>(pow(10, 1 - i));
    }
    for (int i = 2; i < 4; i++) {
        int digit = seconds / static_cast<int>(pow(10, 3 - i));
        timer[i]->sprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32));
        seconds %= static_cast<int>(pow(10, 3 - i));
    }
}

void GameScreen::Validate() {
    if (!Running) { return; }

    int revealedTiles = std::count_if(tiles.begin(), tiles.end(), [](const std::unique_ptr<Tile> &t) {
        return !t->mine && t->revealed;
    });

    int correctlyFlaggedMines = std::count_if(tiles.begin(), tiles.end(), [](const std::unique_ptr<Tile> &t) {
        return t->mine && t->flag;
    });

    if (revealedTiles == total || correctlyFlaggedMines == count) {
        printf("won");
        won = true;
        leaderboardstatus = true;
        FaceButton->texture = winTexture;
    }
}

void GameScreen::addToLeaderboard() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
    string minutes = std::to_string(elapsed.count() / 60);
    string seconds = std::to_string(elapsed.count() % 60);
    if (minutes.size() == 1) minutes = "0" + minutes;
    if (seconds.size() == 1) seconds = "0" + seconds;
    string time = minutes + ":" + seconds;
    string nameAsterick = name + "*"; // Add asterisk to new score
    std::fstream file("files/leaderboard.txt", std::ios::in | std::ios::out);
    std::vector<std::pair<std::string, std::string>> scores;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string time, name;
        if (!(std::getline(iss, time, ',') >> name)) { break; } // error
        // Remove asterisk from old scores
        if (name.back() == '*') {
            name.pop_back();
        }
        scores.emplace_back(time, name);
    }
    file.close(); // Close the file after reading

    scores.emplace_back(time, nameAsterick);
    std::sort(scores.begin(), scores.end());

    if (scores.size() > 5) {
        scores.erase(scores.begin() + 5, scores.end());
    }

    file.open("files/leaderboard.txt",
              std::ios::out | std::ios::trunc); // Open the file in out mode, which truncates the file
    for (const auto &score: scores) {
        file << score.first << ", " << score.second << "\n";
    }
    file.close();
}

void GameScreen::updateTileTexture(bool revealAll) {
    for (auto &tile: tiles) {
        if (revealAll) {
            tile->texture = revealedTexture;
        } else {
            if (tile->revealed) {
                if (tile->mine) {
                    tile->texture = mineTexture;
                } else {
                    if (tile->neighborMines > 0) {
                        std::string filename = "files/images/number_" + std::to_string(tile->neighborMines) + ".png";
                        tile->texture.loadFromFile(filename);
                    } else {
                        tile->texture = revealedTexture;
                    }
                }
            } else if (tile->flag) {
                tile->texture = flagTexture;
            } else {
                tile->texture = hiddenTexture;
            }
        }
    }
}

void GameScreen::Render(sf::RenderWindow &Game) {
    Game.clear(sf::Color::White);
    // displays all the sprites
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            Tile *tile = tiles[i * col + j].get();
            sf::Sprite bottomSprite(tile->bottom);
            bottomSprite.setPosition(j * 32, i * 32);
            Game.draw(bottomSprite);
            sf::Sprite sprite(tile->texture);
            sprite.setPosition(j * 32, i * 32);
            Game.draw(sprite);
        }
    }
    //displays functions
    Game.draw(FaceButton->sprite);
    Game.draw(Debug->sprite);
    Game.draw(PausePlay->sprite);
    Game.draw(leaderboard->sprite);
    for (const auto &i: Counter) {
        Game.draw(i->sprite);
    }
    for (const auto &i: timer) {
        Game.draw(i->sprite);
    }
    Game.display();
    if (leaderboardstatus) {
        leaderboard->onClick();
        leaderboardstatus = false;
        updateTileTexture(false);
    }
};

void GameScreen::handleFlags(Tile *tile) {
    //handles the flags
    tile->flag = true;
    tile->texture = flagTexture;
    tile->bottom = hiddenTexture;
    if (count == 0) { return; }
    // Check if all mines are flagged
    bool allMinesFlagged = true;
    for (auto &t: tiles) {
        if (t->mine && !t->flag) {
            allMinesFlagged = false;
            break;
        }
    }
    count--;
    if (allMinesFlagged) {
        won = true;
        leaderboardstatus = true;
    }
}