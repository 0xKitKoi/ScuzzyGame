#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    std::string room;

    bool inFight = false;
    int enemyID = 0;

    bool textAvailable = false;
    std::vector<std::string> Text;

    std::vector<int> Inventory;

};

// Declare the global GameState instance as extern
extern GameState gameState;

#endif // GAME_STATE_H