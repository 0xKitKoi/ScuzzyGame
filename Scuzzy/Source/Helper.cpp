#include "Helper.hpp"
#include "Math.hpp"
#include "LTexture.hpp"
#include "Entity.hpp"
#include "Enemy.hpp"
#include "NPC.hpp"

#include <string>
#include <vector>
#include "TestNPC.cpp" // i didnt know u can include a cpp file 
#include "Source/Enemies.hpp"

extern std::vector<std::shared_ptr<Entity>> Entities;
extern std::vector<SDL_Rect*> collisionBoxes;
extern std::vector<SDL_Rect> clips;

extern std::shared_ptr<LTexture> getTexture(const std::string& filename);

int LevelIDFromName(std::string name) {
	if (name == "test") {
		return 0;
	}
	else if (name == "Level1") {
		return 1;
	}
	else if (name == "Level2") {
		return 2;
	}
	else if (name == "MLEM") {
		return 3;
	}
	else {
		return -1;
	}
}


Vector2f LoadLevel(std::string Room, LTexture* Map) {
	Entities.clear();
	collisionBoxes.clear();
	


	Vector2f leveldimentions;
	switch (LevelIDFromName(Room)) {
	case 0:
		if (!Map->loadFromFile("data/concept art.bmp")) // "data/concept art.bmp" data/startingalley.png
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {

			// Load first entity , Enemy !
			Vector2f entityPos(950, 390);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 2,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 3,0,128,128 };
			clips.push_back(tmp);
			SDL_Rect entity_cb = { entityPos.x + 25, entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			std::vector<std::string> enemydialogue = { "The Box Full of \"Fuck You\" Appeared!", "The Box of fuck you said ... \"Fuck you\"", "You opened the box. There was \"fuck you\" inside." };
			auto entity = std::make_shared<Entity>(entityPos, entity_cb, entityRect, getTexture("data/box_fuck_u_ari_1.png"), 2, clips, 44);
			
			// create the enemy and bind it to the entity
			//std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity); // make an enemy object initialized with the entity object
			std::shared_ptr<Enemy> child = std::make_shared<BoxOfFuckYou>(entity); // polymorphisim
			entity->setEnemy(child); // bind the new enemy object to the entity

			//entity->m_Enemy->m_EnemyProjectile = std::make_shared<Projectile>(getTexture("data/boolet.png"), {0,0,0,0}, {0,0}, {0,0}, 1);
			


			Entities.push_back(entity); // vector of all entities to render.
			collisionBoxes.push_back(&entity->m_Collider);




			// DOOR TEST
			clips.clear();
			Vector2f doorPos(400, 300);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			entity_cb = { (int)entityPos.x + 25, (int)entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(800, 370);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "Level1", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);



			// first NPC! // I think I'll leave the Signs like this so I can easily edit what they say.
			Vector2f signpos(1000, 1000);
			SDL_Rect signRect = { 0,0,128,128 };
			auto signTexture = getTexture("data/hintsign.png");
			clips.clear();
			clips.push_back({ 0,0,128,128 });
			SDL_Rect signCB = { signpos.x + 25, signpos.y + 25, signRect.w - 45, signRect.h - 55 };
			auto signentity = std::make_shared<Entity>(signpos, signCB, signRect, getTexture("data/hintsign.png"), 1, clips, 2);
			Entities.push_back(signentity);
			std::vector<std::string> dialogue = { "Hello, I'm a fucking sign. ufck you" };
			std::shared_ptr<NPC> signnpc = std::make_shared<SIGNNPC>(dialogue, signentity);
			signentity->setNPC(signnpc);

			collisionBoxes.push_back(&signentity->m_Collider);




			// TEST OF doodoomart box enemy
			Vector2f entityPos2(3000, 390); // Place in the level
			SDL_Rect entityRect2 = { 0,0,128,128 }; // basic size of sprite
			clips.clear(); // rects for sprite sheet animation
			clips = { { 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128 * 3 ,0,128,128 }, { 128 * 4 ,0,128,128 }, { 128 * 5 ,0,128,128 } };

			entity_cb = { (int)entityPos2.x + 25, (int)entityPos2.y + 25, entityRect2.w - 45, entityRect2.h - 55 }; // custom per entity but whatever

			// create the entity object
			auto entity2 = std::make_shared<Entity>(entityPos2, entity_cb, entityRect2, getTexture("data/DooDooMart_StorageBox-Sheet.png"), 5, clips, 58);
			// create the enemy and bind it to the entity
			std::shared_ptr<Enemy> child2 = std::make_shared<DooDooMartBox>(entity2);

			entity2->setEnemy(child2); // bind the new enemy object to the entity
			Entities.push_back(entity2); // vector of all entities to render.
			collisionBoxes.push_back(&entity2->m_Collider);

			// barrel of toxic waste because why not
			clips.clear();
			clips.push_back({ 0,0,128,128 });
			auto barrel = std::make_shared<Entity>(Vector2f(2322, 258), SDL_Rect{ 0,0,128,128 }, SDL_Rect{ 0,0,128,128 }, getTexture("data/barrel_nuclear.png"), 1, clips, 1);
			Entities.push_back(barrel);



		}


		break;
	case 1:
		if (!Map->loadFromFile("data/darkalley.png")) // "data/startingalley.png"
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			// DOOR TEST
			Vector2f entityPos(872, 770);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp, entity_cb;
			clips.clear();
			Vector2f doorPos(872, 770);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			entity_cb = { (int)entityPos.x + 25, (int)entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			auto Doorentity2 = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity2); // vector of all entities to render.
			Vector2f outpos(540, 300);//(1000, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity2, "MLEM", outpos);
			doornpc->m_Entity = Doorentity2;
			Doorentity2->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity2->m_Collider);



			/*
			Vector2f entityPos(950, 390);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 2,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 3,0,128,128 };
			clips.push_back(tmp);
			SDL_Rect entity_cb = { entityPos.x + 25, entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			std::vector<std::string> enemydialogue = { "The Box Full of \"Fuck You\" Appeared!", "The Box of fuck you said ... \"Fuck you\"", "You opened the box. There was \"fuck you\" inside." };
			auto entity = std::make_shared<Entity>(entityPos, entity_cb, entityRect, getTexture("data/box_fuck_u_ari_1.png"), 2, clips, 44);
			// create the enemy and bind it to the entity
			std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity); // make an enemy object initialized with the entity object
			child->m_AttackDamage = 1;
			entity->setEnemy(child); // bind the new enemy object to the entity
			entity->m_Enemy->m_EnemyDialogue = enemydialogue;
			entity->m_Enemy->m_Actions = { "info", "sit", "kick.?" };
			entity->m_Enemy->m_ActionResponse = { "STATUS: .. its a box..?", "You sat on the box, it left a dent in it.", "WHAM! you left a big dent in its fleshy cardboard." };
			Entities.push_back(entity); // vector of all entities to render.
			collisionBoxes.push_back(&entity->m_Collider);
			
			*/


			// TEST OF doodoomart box enemy
			Vector2f entityPos2(950, 390);
			SDL_Rect entityRect2 = { 0,0,128,128 };
			clips.clear();
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 2,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128*3 ,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 4 ,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128 * 5 ,0,128,128 };
			clips.push_back(tmp);
			entity_cb = { (int)entityPos2.x + 25, (int)entityPos2.y + 25, entityRect2.w - 45, entityRect2.h - 55 }; // custom per entity but whatever
			std::vector<std::string> enemydialogue = { "The DOODOOMART Box ran at you!", "The DooDoo Mart Box has a buncha doodoo init", "The doodoomart box gave you a negative coupon. you are now in even more doodoo debt." };
			auto entity = std::make_shared<Entity>(entityPos2, entity_cb, entityRect2, getTexture("data/DooDooMart_StorageBox-Sheet.png"), 5, clips, 58);
			std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity);
			child->m_AttackDamage = 3;
			entity->setEnemy(child); // bind the new enemy object to the entity
			entity->m_Enemy->m_EnemyDialogue = enemydialogue;
			entity->m_Enemy->m_Actions = { "info", "dissassemble", "turn into shitbox" };
			entity->m_Enemy->m_ActionResponse = { "STATUS: .. its a box..?", "You flattened the box. It took Heavy Damage", "my actual pc" };
			entity->m_Enemy->m_EnemyFightSpriteSheet = getTexture("data/DooDooMart_StorageBox-Sheet.png");
			entity->m_Enemy->FRAME_COUNT = 2;
			entity->m_Enemy->m_EnemySpriteClips = clips;
			Entities.push_back(entity); // vector of all entities to render.
			collisionBoxes.push_back(&entity->m_Collider);



		}
		// TODO: Position relative to the window? or the actual map
		// door position should be the same on all screen sizes.






		break;
	case 2:
		if (!Map->loadFromFile("data/fight convepts.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			// DOOR TEST
			Vector2f entityPos(950, 490);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp = { 0,0,128,128 };
			clips.clear();
			Vector2f doorPos(400, 300);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			SDL_Rect entity_cb = { entityPos.x + 25, entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(960, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "test", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);
		}
		break;

	case 3:
		if (!Map->loadFromFile("data/MLEM.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			// DOOR TEST
			Vector2f entityPos(950, 490);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp = { 0,0,128,128 };
			clips.clear();
			Vector2f doorPos(400, 300);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			SDL_Rect entity_cb = { entityPos.x + 25, entityPos.y + 25, entityRect.w - 45, entityRect.h - 55 }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(960, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "test", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);
		}
		break;
	default:
		if (!Map->loadFromFile("data/Error.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		break;
	}
	leveldimentions.x = Map->getHeight();
	leveldimentions.y = Map->getWidth(); // callers problem to load level dimensions. i aint doing all that.
	return leveldimentions;
}

std::string GetItemnameFromIndex(int index) {
	switch (index) {
	case 0:
		return "Healing item TEST";
		break;
	case 1:
		return "money giver test";
		break;
	default:
		//printf("\n [!] ERROR: Could not get Item name at selection index: %d", index);
		return "ERROR";
		break;
	}
}

// time to implement the item system.

/// <summary>
/// Manipulates the GameState Object based on ID. Caller is responcible for deleting the item from inventory.
/// </summary>
/// <param name="ID">Item ID</param>
/// <returns>(int)Success</returns>
int UseItem(int ID) {
	int ret = -1;
	switch (ID) {
	case 0:
		printf("You used Test Item 1!\n");
		gameState.HP += 10; // heal 10 HP
		ret = 0;
		break;
	case 1:
		printf("You used Test Item 2!\n");
		gameState.money += 5;
		ret = 0;
		break;
	}
	return ret;
}

std::string GetItemDescription(int ID) {
	switch (ID) {
	case 0:
		return "A test item that heals you for 10 HP.";
		break;
	case 1:
		return "A test item that gives you 5 money.";
		break;
	default:
		return "ERROR: No description found for this item.";
		break;
	}
}