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
	DODGE_MECHANIC,    // Enemy attack dodge mechanic
    ENEMY_DIALOGUE,     // Enemy speaking dialogue
    RESULT_DIALOGUE,    // Showing results of an action
    FIGHT_END,          // Fight is over (win/lose)
	PLAYER_FIGHT,       // Player is timing an attack
	PLAYER_MAGIC,       // Player is selecting ability.
	TUTORIAL_PROMPT     // Wizard explains the next required tutorial input.

};


#endif // !FightSystem

#ifndef ENUMS_HPP
#define ENUMS_HPP
// Soul Ripping Mechanic
enum class EncounterPhase { NONE, SOUL_PULL, SOUL_LAUNCH };

// SoulRubberBandBall Menu Direction Enum
enum class BackLayerDirection { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};

    enum class MerchantReaction {
        Greet, OfferTalk, OfferBuy, Decline, TalkBack,
        PurchaseSuccess, CantAfford,
    };

#endif // !ENUMS_HPP
