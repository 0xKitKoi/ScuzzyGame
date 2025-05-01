#include "Helper.hpp"
#include "Math.hpp"
#include "LTexture.hpp"
#include "Entity.hpp"
#include "Enemy.hpp"
#include "NPC.hpp"

#include <string>
#include <vector>
#include "TestNPC.cpp" // i didnt know u can include a cpp file 

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
			std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity); // make an enemy object initialized with the entity object
			child->m_AttackDamage = 1;
			entity->setEnemy(child); // bind the new enemy object to the entity
			entity->m_Enemy->m_EnemyDialogue = enemydialogue;
			entity->m_Enemy->m_Actions = { "info", "sit", "kick.?" };
			entity->m_Enemy->m_ActionResponse = { "STATUS: .. its a box..?", "You sat on the box, it left a dent in it.", "WHAM! you left a big dent in its fleshy cardboard." };
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
			Vector2f outpos(960, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "Level1", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);

			// first NPC! 
			Vector2f signpos(1000, 1000);
			SDL_Rect signRect = { 0,0,128,128 };
			auto signTexture = getTexture("data/hintsign.png");
			clips.clear();
			clips.push_back({ 0,0,128,128 });
			SDL_Rect signCB = { signpos.x + 25, signpos.y + 25, signRect.w - 45, signRect.h - 55 };
			auto signentity = std::make_shared<Entity>(signpos, signCB, signRect, getTexture("data/hintsign.png"), 1, clips, 2);
			Entities.push_back(signentity);
			std::vector<std::string> dialogue = { "Hello, I'm a fucking sign" };
			std::shared_ptr<NPC> signnpc = std::make_shared<SIGNNPC>(dialogue, signentity);
			signentity->setNPC(signnpc);

			collisionBoxes.push_back(&signentity->m_Collider);
		}


		break;
	case 1:
		if (!Map->loadFromFile("data/startingalley.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		break;
	case 2:
		if (!Map->loadFromFile("data/fight convepts.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			// DOOR TEST
			Vector2f entityPos(950, 390);
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
