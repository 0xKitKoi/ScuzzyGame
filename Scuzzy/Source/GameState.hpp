#include "Source/NPC.hpp"


#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    std::string room;
    int money = 0;
    int kills = 0;

    bool OpenedMenu = false; // This is the player's ingame menu for items and talking.

    bool inMenu = false; // in a dialogue box with chioces
    int selectionIndex = 0;
    NPC* callbackNPC = nullptr;
    

    bool inFight = false;
    int enemyID = 0;

    bool textAvailable = false;
    std::vector<std::string> Text;
    int textIndex = 0;
    
    

    std::vector<int> Inventory;

};

// Declare the global GameState instance as extern
extern GameState gameState;


#endif // GAME_STATE_H