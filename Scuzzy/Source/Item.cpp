#include "Source/Item.hpp"
#include "Source/GameState.hpp"
#include "Source/TestNPC.hpp"

extern std::vector<std::shared_ptr<Entity>> Entities;

int BandAid::Use() {
    gameState.HP += 5;
    return 1;
}

int Key::Use() {
    //gameState.money += 10;
	// m_DoorID
	for (auto& entity : Entities)
	{
		auto doorNPC = std::dynamic_pointer_cast<DoorNPC>(entity->m_NPC);

		if (doorNPC && doorNPC->m_DoorID == m_DoorID)
		{
			doorNPC->m_Unlocked = true;

			printf("Door with ID %d unlocked!\n", m_DoorID);

			// triggler dialogue about unlocking door.
			gameState.Text = { "You used the key to unlock the door!" };
			gameState.textAvailable = true;
			gameState.shouldAnimateText = true;
			gameState.currentDisplayText = "";
			gameState.textSoundEffectTick = 0; // reset text sound effect timer for new dialogue

			return 0;
		}
	}
	// no door was found.
	gameState.Text = { "You used the key, but nothing happened." };
	gameState.textAvailable = true;
	gameState.shouldAnimateText = true;
	gameState.currentDisplayText = "";
	gameState.textSoundEffectTick = 0; // reset text sound effect timer for new dialogue

    return 1;
}
