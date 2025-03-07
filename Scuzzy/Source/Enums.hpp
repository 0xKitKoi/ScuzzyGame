#ifndef FightSystem
#define FightSystem

// Define the fight states
enum class FightState {
    INTRO,              // Initial dialogue when fight starts
    PLAYER_TURN_MENU,   // Player selecting from main menu (Fight/Actions/Items)
    PLAYER_ACTIONS_MENU, // Player selecting an action
    PLAYER_ITEMS_MENU,   // Player selecting an item
    PLAYER_ACTION_RESULT, // Showing result of player's action
    ENEMY_TURN,         // Enemy deciding and taking action
    ENEMY_DIALOGUE,     // Enemy speaking dialogue
    RESULT_DIALOGUE,    // Showing results of an action
    FIGHT_END           // Fight is over (win/lose)
};




#endif // !FightSystem
