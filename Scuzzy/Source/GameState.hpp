#include "Source/NPC.hpp"
#include "Source/Enemy.hpp"
#include <Source/Entity.hpp>

#include "Source/Enums.hpp"

//enum class FightState {
//    INTRO,              // Initial dialogue when fight starts
//    PLAYER_TURN_MENU,   // Player selecting from main menu (Fight/Actions/Items)
//    PLAYER_ACTIONS_MENU, // Player selecting an action
//    PLAYER_ITEMS_MENU,   // Player selecting an item
//    PLAYER_ACTION_RESULT, // Showing result of player's action
//    ENEMY_TURN,         // Enemy deciding and taking action
//    ENEMY_DIALOGUE,     // Enemy speaking dialogue
//    RESULT_DIALOGUE,    // Showing results of an action
//    FIGHT_END           // Fight is over (win/lose)
//};



#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
    //std::vector<std::shared_ptr<Entity>> Entities;


    std::string room;
    int money = 0;
    int kills = 0;
    int HP = 10;

    // Flags
	FightState fightState;
    bool DebugMode = true; // Debug mode for testing
    
	bool LoadingScreen = false; // Loading screen for room transitions
	bool DoneLoading = false;
    bool fade = true;
    bool OpenedMenu = false; // This is the player's ingame menu for items and talking.
    bool textAvailable = false;
    bool inMenu = false; // in a dialogue box with chioces
	bool wonFight = false;
    bool inFight = false;
    bool FightStarted = false;
    bool TURN = false;
    int turnCount = 0;
    int Plot = 0;
	bool ActionsMenu = false;
    bool ActionResponse = false;

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