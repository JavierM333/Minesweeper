#include <SFML/Graphics.hpp>
#include "Welcome.h"
#include "GameScreen.h"
#include "Leaderboard.h"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    ifstream file("files/config.cfg", ios::in);
    if (!file.is_open()) {
        cout << "Error: config.cfg not found!" << endl;
        return 0;
    }

    int row, col, mines;
    file >> col >> row >> mines;
    file.close();

    if (mines < 1 || row < 1 || col < 1 || mines > row * col) {
        cout << "Error: Invalid config.cfg file or too many mines!" << endl;
        return 0;
    }

    if (Welcome(row, col).isClosed()) { return 0; }
    GameScreen game(row, col, mines);
    while (game.getRestartStatus()) {
        game = GameScreen(row, col, mines);
    }
    return 0;
}