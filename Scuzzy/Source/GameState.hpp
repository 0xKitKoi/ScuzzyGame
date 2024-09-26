#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    bool inFight;
    int enemyID;
};

// Declare the global GameState instance as extern
extern GameState gameState;

#endif // GAME_STATE_H