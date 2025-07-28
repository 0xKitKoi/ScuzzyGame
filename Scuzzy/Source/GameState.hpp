#include "Source/NPC.hpp"
#include "Source/Enemy.hpp"
#include <Source/Entity.hpp>
#include <Source/Player.hpp>
#include "Source/Enums.hpp"
#include <memory>

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

	//std::shared_ptr<Player> player = nullptr;
	Player* player = nullptr;

    std::string room;
    int money = 0;
    int kills = 0;
    int HP = 10;
    bool dead = false;

    Vector2f DOOR_OUT_POS = {0,0};

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
    int actionindex = 0;

    //bool textAvailable = false;
    std::vector<std::string> Text;
    int textIndex = 0;
    
    // Text animation state
    std::string currentDisplayText;  // The text currently being displayed
    int currentCharIndex = 0;        // Current character index in the animation
    float textTimer = 0.0f;          // Timer for text animation
    float textSpeed = 0.03f;         // Speed of text animation (seconds per character)
    bool textAnimating = false;      // Whether text is currently animating
    bool shouldAnimateText = false;  // Whether the current text should be animated (dialogue vs menu)
    
    std::vector<int> Inventory;

};

// Declare the global GameState instance as extern
extern GameState gameState;


#endif // GAME_STATE_H