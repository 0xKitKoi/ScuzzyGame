#include "Source/NPC.hpp"
#include "Source/Enemy.hpp"
#include <Source/Entity.hpp>


#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    //std::vector<std::shared_ptr<Entity>> Entities;


    std::string room;
    int money = 0;
    int kills = 0;

    // Flags
    bool OpenedMenu = false; // This is the player's ingame menu for items and talking.
    bool textAvailable = false;
    bool inMenu = false; // in a dialogue box with chioces
    bool inFight = false;
    bool checkFlag = false;

    int selectionIndex = 0;
    NPC* callbackNPC = nullptr;
    

    //bool inFight = false;
    int enemyID = 0;
    Enemy* enemy = nullptr;

    //bool textAvailable = false;
    std::vector<std::string> Text;
    int textIndex = 0;
    
    

    std::vector<int> Inventory;

};

// Declare the global GameState instance as extern
extern GameState gameState;


#endif // GAME_STATE_H