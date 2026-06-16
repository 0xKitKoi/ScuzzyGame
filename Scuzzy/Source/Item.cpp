#include "Source/Item.hpp"
#include "Source/GameState.hpp"
#include "Source/TestNPC.hpp"

extern std::vector<std::shared_ptr<Entity>> Entities;

int BandAid::Use() {
	if (gameState.HP >= gameState.maxHP) {
			gameState.Text = { "You dont need to use that right now." };
			//gameState.textAvailable = true; // no need to, already handled.
			gameState.shouldAnimateText = true;
			gameState.currentDisplayText = "";
			gameState.textSoundEffectTick = 0; // reset text sound effect timer for new dialogue
		return 1;
	}
    gameState.HP += 5;
	// TODO: healing sound effect here. 
    return 0;
}

int Key::Use() {
    //gameState.money += 10;
	// m_DoorID
	// this finds the door using the Entities Array. I dont know if I want that.
	// Since the player has to go to the door, should simply having the key unlock it already ? or should the player use the key at the door?
	// right now the player gets the key item, and using it anywhere will unlock the door. But it has to be in the current level. 
	// The Key item holds the Door's ID, so the door doesnt have to be a door, it could be a puzzle.
	for (auto& entity : Entities)
	{
		auto doorNPC = std::dynamic_pointer_cast<DoorNPC>(entity->m_NPC);

		if (doorNPC && doorNPC->m_DoorID == m_DoorID)
		{
			doorNPC->m_Unlocked = true;

			printf("Door with ID %d unlocked!\n", m_DoorID);

			// triggler dialogue about unlocking door.
			gameState.Text = { "You used the key to unlock the door!" };
			//gameState.textAvailable = true; // no need to trigger a text box, the MenuSystem goes to the response already.
			gameState.shouldAnimateText = true;
			gameState.currentDisplayText = "";
			gameState.textSoundEffectTick = 0; // reset text sound effect timer for new dialogue
			// TODO: Door sound effects here?
			return 0;
		}
	}
	// no door was found.
	gameState.Text = { "You used the key, but nothing happened." };
	//gameState.textAvailable = true;
	gameState.shouldAnimateText = true;
	gameState.currentDisplayText = "";
	gameState.textSoundEffectTick = 0; // reset text sound effect timer for new dialogue

    return 1;
}
