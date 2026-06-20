#include "Helper.hpp"
#include "Math.hpp"
#include "LTexture.hpp"
#include "Entity.hpp"
#include "Enemy.hpp"
#include "NPC.hpp"

#include <string>
#include <vector>
#include "TestNPC.hpp" // i didnt know u can include a cpp file 
#include "Source/Enemies.hpp"
#include "Source/GameState.hpp"
#include "Source/Item.hpp"

#include <random>

extern std::vector<std::shared_ptr<Entity>> Entities;
extern std::vector<SDL_Rect*> collisionBoxes;
extern std::vector<SDL_Rect> staticCollisionBoxes;
extern std::vector<SDL_Rect> clips;

extern Mix_Chunk* gExplosionSound;

extern Camera camera;

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
	else if (name == "MAGICANT") {
		return 4;
	}
	else if (name == "testtiled") {
		return 5;
	}
	else {
		return -1;
	}
}


Vector2f LoadLevel(std::string Room, LTexture* Map) {
	Entities.clear();
	collisionBoxes.clear();
	staticCollisionBoxes.clear();
	


	Vector2f leveldimentions;
	switch (LevelIDFromName(Room)) {
	case 0:
		if (!Map->loadFromFile("data/concept art.bmp")) // "data/concept art.bmp" data/startingalley.png
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {

			gameState.mapScaling = 1.0f;
			//Vector2f PuddlePos(1400, 700);
			//SDL_Rect puddleRect = { 0, 0, 200, 200 };
			//clips.clear();
			//clips.push_back({ 0, 0, 200, 200 });
			//clips.push_back({ 0,200,200, 200 });
			//clips.push_back({ 0,200*2,200, 200 });
			//clips.push_back({ 200,0,200, 200 });
			//clips.push_back({ 200,200,200, 200 });
			//clips.push_back({ 200,200*2,200, 200 });
			//clips.push_back({ 200*2,0,200, 200 });
			//clips.push_back({ 200 * 2,200,200, 200 });
			//clips.push_back({ 200 * 2,200*2,200, 200 });
			//SDL_Rect puddleCB = { PuddlePos.x, PuddlePos.y, 200, 200 };
			//auto PuddleEntity = std::make_shared<Entity>(PuddlePos, puddleCB, puddleRect, getTexture("data/Puddle.png"), 7, clips, 69);
			//Entities.push_back(PuddleEntity);
			//PuddleEntity->moving = true;
			//Vector2f PuddleOutPos(400, 200);
			//std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "MAGICANT", PuddleOutPos);
			//PuddleNPC->m_Entity = PuddleEntity;
			//PuddleEntity->setNPC(PuddleNPC);
			//collisionBoxes.push_back(&PuddleEntity->m_Collider);





			/*
						// DOOR TEST
			clips.clear();
			Vector2f doorPos(400, 300);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(400, 200);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "Level1", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);

			*/


			// CutScene trigger.
			Vector2f triggerPos(1300, 1100);
			SDL_Rect triggerRect = { 0, 0, 128, 128 };
			SDL_Rect triggerCB = { triggerPos.x, triggerPos.y, 128, 128 };
			//auto triggerEntity = std::make_shared<Entity>(triggerPos, triggerCB, triggerRect, 1, 401);
			auto triggerEntity = std::make_shared<Entity>(
				triggerPos, triggerCB, triggerRect,
				nullptr,           // or a real texture
				1,                 // framecount
				std::vector<SDL_Rect>{{0,0,0,0}},  // clips
				401                // EntityID
			);
			Entities.push_back(triggerEntity);
			//collisionBoxes.push_back(&triggerEntity->m_Collider);
			std::vector<std::unique_ptr<CutsceneAction>> cutsceneActions;
			cutsceneActions.push_back(std::make_unique<DialogueAction>(gameState, std::vector<std::string>{"You stepped on a cutscene trigger!", "you stepped on it so hard that it died."}));
			cutsceneActions.push_back(std::make_unique<MoveEntityAction>(triggerEntity.get(), Vector2f(800, 300), 100.0f));
			std::shared_ptr<NPC> triggerNPC = std::make_shared<TriggerNPC>(triggerEntity, triggerCB, std::move(cutsceneActions));
			triggerEntity->setNPC(triggerNPC);


						// CutScene trigger2 
			Vector2f triggerPos2(2000, 1100);
			SDL_Rect triggerRect2 = { 0, 0, 72, 100 };
			SDL_Rect triggerCB2 = { triggerPos2.x, triggerPos2.y, 128, 128 };
			std::vector<SDL_Rect> explClips;
			for (int i = 0; i < 15; i++) {
				explClips.push_back( SDL_Rect{ 72*i, 0, 72, 100});
			}

			//auto triggerEntity = std::make_shared<Entity>(triggerPos, triggerCB, triggerRect, 1, 401);
			auto triggerEntity2 = std::make_shared<Entity>(
				triggerPos2, triggerCB2, triggerRect2,
				nullptr, //getTexture("data/RealisticExplosion72x100x18.png"), //nullptr,           // or a real texture 
				1,                 // framecount
				explClips,  // clips
				402                // EntityID
			);
			Entities.push_back(triggerEntity2);
			//collisionBoxes.push_back(&triggerEntity->m_Collider);
			std::vector<std::unique_ptr<CutsceneAction>> cutsceneActions2;
			cutsceneActions2.push_back(std::make_unique<DialogueAction>(gameState, std::vector<std::string>{"You stepped on a cutscene trigger!", "you stepped on it so hard that it died."}));
			cutsceneActions2.push_back(std::make_unique<ExplosionAction>(gExplosionSound, getTexture("data/RealisticExplosion72x100x18.png"), 15, explClips, triggerPos2));
			std::shared_ptr<NPC> triggerNPC2 = std::make_shared<TriggerNPC>(triggerEntity2, triggerCB2, std::move(cutsceneActions2));
			triggerEntity2->setNPC(triggerNPC2);
		




			// Merchant Test:
			Vector2f shopguyPOS = { 1200, 600 };
			SDL_Rect shopguyRect = {0, 0, 128, 128} ;
			clips.clear();
			clips.push_back({0, 0, 128, 128});
			clips.push_back({128, 0, 128, 128});
			SDL_Rect shopGuyCollisionBox = {shopguyPOS.x, shopguyPOS.y, 128, 128};
			auto MerchantGuy = std::make_shared<Entity> (shopguyPOS, shopGuyCollisionBox, shopguyRect, 
			getTexture("data/AverageTweaker.png"),
			2,
			clips,
			420
			);
			Entities.push_back(MerchantGuy);	
			collisionBoxes.push_back(&MerchantGuy->m_Collider);
			std::vector<std::string> SHOPdialogue = {"GETT OUUTA ME HEADD!!!"};
			std::vector<MerchantNPC::ShopItem> stock = {MerchantNPC::ShopItem{2, 2, "test"}, MerchantNPC::ShopItem{1, 2, "weenie"} };
			std::shared_ptr<MerchantNPC> shopdood = std::make_shared<MerchantNPC>(SHOPdialogue, MerchantGuy, stock);
			MerchantGuy->setNPC(shopdood);


			// Dr.Pebba sodacan enemy test
			Vector2f sodaPos(3841, 314);
			SDL_Rect sodaRect = { 0, 0, 128, 128 };

			clips.clear();
			clips.push_back({ 0, 0, 128, 128 });

			SDL_Rect sodaCB = {
				int(sodaPos.x + 8),
				int(sodaPos.y + 10),
				48,	
				44
			};

			auto sodaCanEntity = std::make_shared<Entity>(
				sodaPos,
				sodaCB,
				sodaRect,
				getTexture("data/drpebba.png"),
				1,
				clips,
				0
			);

			std::shared_ptr<Enemy> sodaCanEnemy = std::make_shared<DrPebba>(sodaCanEntity);
			sodaCanEntity->setEnemy(sodaCanEnemy);

			Entities.push_back(sodaCanEntity);
			collisionBoxes.push_back(&sodaCanEntity->m_Collider);

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
			SDL_Rect entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
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
			entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(400, 200);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "Level1", outpos, 81); // DOOR ID 81 added. important for keys!
			
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);
			//Doorentity->m_NPC->m_Unlocked = false; // testing locked door
			std::shared_ptr<DoorNPC> door =
				std::dynamic_pointer_cast<DoorNPC>(doornpc);
			door->m_Unlocked = false; // testing locked door, Cast back to DoorNPC.
			//MerchantNPC sells key with ID 81, so this door should unlock when the player buys the key from the merchant and uses it.



			// first NPC! // I think I'll leave the Signs like this so I can easily edit what they say.
			Vector2f signpos(1000, 1000);
			SDL_Rect signRect = { 0,0,128,128 };
			auto signTexture = getTexture("data/hintsign.png");
			clips.clear();
			clips.push_back({ 0,0,128,128 });
			SDL_Rect signCB = { int(signpos.x + 25), int(signpos.y + 25), int(signRect.w - 45), int(signRect.h - 55) };
			auto signentity = std::make_shared<Entity>(signpos, signCB, signRect, getTexture("data/hintsign.png"), 1, clips, 2);
			Entities.push_back(signentity);
			std::vector<std::string> dialogue = { "Hello, I'm a fucking sign. ufck you" ,
				 "TODO: 1) better projectiles, since bounding box was implimented.",
				 "2) CutScenes Implimented, so make some god damn cutscenes.",
				 "3) Wacky Dreamlike Situations, word play. ",
				 "4) make some god damn maps ",
				 " 5) what the hell is the main story",
				 "6) fight action menu needs heavy rework. Make undertale/Deltarune style actions actually do somehting. ",
				 "7) decide if sparing will be a thing here."
				};
			std::shared_ptr<NPC> signnpc = std::make_shared<SIGNNPC>(dialogue, signentity);
			signentity->setNPC(signnpc);

			collisionBoxes.push_back(&signentity->m_Collider);



			// TEST OF doodoomart box enemy
			Vector2f entityPos2(3000, 390); // Place in the level
			SDL_Rect entityRect2 = { 0,0,128,128 }; // basic size of sprite
			clips.clear(); // rects for sprite sheet animation
			clips = { { 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128 * 3 ,0,128,128 }, { 128 * 4 ,0,128,128 }, { 128 * 5 ,0,128,128 } };

			entity_cb = { int(entityPos2.x + 25), int(entityPos2.y + 25), int(entityRect2.w - 45), int(entityRect2.h - 55) }; // custom per entity but whatever

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

			

			// Heres where I want to define custom collision boxes:
			SDL_Rect leftWall = { 300, 600, 40, 400 };
			SDL_Rect topWall = { 300, 600, 340, 40 };
			SDL_Rect rightWall = { 640, 600, 40, 400 };
			SDL_Rect bottomWall = { 300, 960, 340, 40 };
				
				staticCollisionBoxes.push_back(leftWall);

				staticCollisionBoxes.push_back(topWall);

				staticCollisionBoxes.push_back(rightWall);

				staticCollisionBoxes.push_back(bottomWall);

		}


		break;
	case 1:
		if (!Map->loadFromFile("data/darkalley.png")) // "data/startingalley.png"
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {

			gameState.mapScaling = 1.1f;


			// DOOR TEST
			Vector2f entityPos(450, 770);
			SDL_Rect entityRect = { 0,0,128,128 };
			SDL_Rect tmp, entity_cb;
			clips.clear();
			Vector2f doorPos(472, 770);
			entityRect = { 0,0,128,128 };
			tmp = { 0,0,128,128 };
			clips.push_back(tmp);
			tmp = { 128,0,128,128 };
			clips.push_back(tmp);
			entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
			auto Doorentity2 = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity2); // vector of all entities to render.
			Vector2f outpos(540, 300);//(1000, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity2, "MAGICANT", outpos);
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
			Vector2f entityPos3(450, 500);
			SDL_Rect entityRect2 = { 0,0,128,128 };
			clips.clear();
			clips = {{ 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128*3 ,0,128,128 }, { 128 * 4 ,0,128,128 }, { 128 * 5 ,0,128,128 }};
			//tmp = { 0,0,128,128 };, 
			//clips.push_back(tmp);
			//tmp = { 128,0,128,128 };
			//clips.push_back(tmp);
			//tmp = { 128 * 2,0,128,128 };
			//clips.push_back(tmp);
			//tmp = { 128*3 ,0,128,128 };
			//clips.push_back(tmp);
			//tmp = { 128 * 4 ,0,128,128 };
			//clips.push_back(tmp);
			//tmp = { 128 * 5 ,0,128,128 };
			//clips.push_back(tmp);
			entity_cb = { int(entityPos3.x + 25), int(entityPos3.y + 25), int(entityRect2.w - 45), int(entityRect2.h - 55) }; // custom per entity but whatever
			std::vector<std::string> enemydialogue = { "The DOODOOMART Box ran at you!", "The DooDoo Mart Box has a buncha doodoo init", "The doodoomart box gave you a negative coupon. you are now in even more doodoo debt." };
			auto entity = std::make_shared<Entity>(entityPos3, entity_cb, entityRect2, getTexture("data/DooDooMart_StorageBox-Sheet.png"), 5, clips, 58);
			//std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity);
			std::shared_ptr<Enemy> child = std::make_shared<DooDooMartBox>(entity);
			//child->m_AttackDamage = 3;
			entity->setEnemy(child); // bind the new enemy object to the entity
			//entity->m_Enemy->m_EnemyDialogue = enemydialogue;
			//entity->m_Enemy->m_Actions = { "info", "dissassemble", "turn into shitbox" };
			//entity->m_Enemy->m_ActionResponse = { "STATUS: .. its a box..?", "You flattened the box. It took Heavy Damage", "my actual pc" };
			//entity->m_Enemy->m_EnemyFightSpriteSheet = getTexture("data/DooDooMart_StorageBox-Sheet.png");
			//entity->m_Enemy->FRAME_COUNT = 2;
			//entity->m_Enemy->m_EnemySpriteClips = clips;
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
			SDL_Rect entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
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
			gameState.mapScaling = 1.2f;

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
			SDL_Rect entity_cb = { int(entityPos.x + 25), int(entityPos.y + 25), int(entityRect.w - 45), int(entityRect.h - 55) }; // custom per entity but whatever
			auto Doorentity = std::make_shared<Entity>(doorPos, entity_cb, entityRect, getTexture("data/door.png"), 2, clips, 69);
			Entities.push_back(Doorentity); // vector of all entities to render.
			Vector2f outpos(1200, 700);

			/*
			    Parent parent;

				// Create a Child and pass the parent to it
				std::shared_ptr<Child> child = std::make_shared<Child>(parent);

				// Set the child in the parent
				parent.setChild(std::move(child));

				// Call the child's function through the parent
				parent.callChildFunction();
			*/

			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "test", outpos);
			doornpc->m_Entity = Doorentity;
			Doorentity->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity->m_Collider);



			Vector2f PuddlePos(1025, 420);
			SDL_Rect puddleRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			clips.push_back({ 0,200 * 2,200, 200 });
			clips.push_back({ 200,0,200, 200 });
			clips.push_back({ 200,200,200, 200 });
			clips.push_back({ 200,200 * 2,200, 200 });
			clips.push_back({ 200 * 2,0,200, 200 });
			clips.push_back({ 200 * 2,200,200, 200 });
			clips.push_back({ 200 * 2,200 * 2,200, 200 });
			SDL_Rect puddleCB = { PuddlePos.x, PuddlePos.y, 200, 200 };
			auto PuddleEntity = std::make_shared<Entity>(PuddlePos, puddleCB, puddleRect, getTexture("data/Puddle.png"), 7, clips, 69);
			Entities.push_back(PuddleEntity);
			PuddleEntity->moving = true;
			Vector2f PuddleOutPos(400, 200);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "MAGICANT", PuddleOutPos);
			PuddleNPC->m_Entity = PuddleEntity;
			PuddleEntity->setNPC(PuddleNPC);
			collisionBoxes.push_back(&PuddleEntity->m_Collider);
		}
		break;

	case 4:
		if (!Map->loadFromFile("data/Magicant.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.0f;

			// Exit Puddle
			Vector2f PuddlePos(500, 700);
			SDL_Rect puddleRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			clips.push_back({ 0,200 * 2,200, 200 });
			clips.push_back({ 200,0,200, 200 });
			clips.push_back({ 200,200,200, 200 });
			clips.push_back({ 200,200 * 2,200, 200 });
			clips.push_back({ 200 * 2,0,200, 200 });
			clips.push_back({ 200 * 2,200,200, 200 });
			clips.push_back({ 200 * 2,200 * 2,200, 200 });
			SDL_Rect puddleCB = { PuddlePos.x, PuddlePos.y, 200, 200 };
			auto PuddleEntity = std::make_shared<Entity>(PuddlePos, puddleCB, puddleRect, getTexture("data/Puddle.png"), 7, clips, 69);
			Entities.push_back(PuddleEntity);
			Vector2f PuddleOutPos(400, 200);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "testtiled", PuddleOutPos);
			PuddleNPC->m_Entity = PuddleEntity;
			PuddleEntity->moving = true;
			PuddleEntity->setNPC(PuddleNPC);
			collisionBoxes.push_back(&PuddleEntity->m_Collider);

			// Fragment Sheet
			Vector2f FragmentPos(330, 520);
			SDL_Rect FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			SDL_Rect FragmentCB = { FragmentPos.x, FragmentPos.y, 200, 200 };
			auto FragmentSheetEntity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/InstanceSheet.png"), 2, clips, 133);
			Entities.push_back(FragmentSheetEntity);
			std::vector<std::string> FragmentDialogue = { "Looking at the sheets, you see an instance of a memory.", "You feel exactly like how you felt at that slice of time.", "You are disgusted with yourself. Absolutely."};
			std::shared_ptr<NPC> FragmentNPC = std::make_shared<SIGNNPC>(FragmentDialogue, FragmentSheetEntity);
			FragmentNPC->m_Entity = FragmentSheetEntity;
			FragmentSheetEntity->moving = true;
			FragmentSheetEntity->setNPC(FragmentNPC);
			collisionBoxes.push_back(&FragmentSheetEntity->m_Collider);

			// Fragment NPC1
			FragmentPos = { 430, 240 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment1Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment1.png"), 2, clips, 21);
			Entities.push_back(Fragment1Entity);
			std::vector<std::string> Fragment1Dialogue = { "You hear noise that sounds like your voice. You cant make out what they are saying. ", "They, being you. Parts of you.", "You think you heard this but not sure: \"....believe he did that..\""};
			std::shared_ptr<NPC> Fragment1NPC = std::make_shared<SIGNNPC>(Fragment1Dialogue, Fragment1Entity);
			Fragment1NPC->m_Entity = Fragment1Entity;
			Fragment1Entity->moving = true;
			Fragment1Entity->setNPC(Fragment1NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);


			// Fragment NPC2
			FragmentPos = { 630, 240 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment2Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment2.png"), 2, clips, 22);
			Entities.push_back(Fragment2Entity);
			std::vector<std::string> Fragment2Dialogue = { "You cant make out the words.", "You can tell that it is talking down on what you did.", "Even though they agreed with you in the moment."};
			std::shared_ptr<NPC> Fragment2NPC = std::make_shared<SIGNNPC>(Fragment2Dialogue, Fragment2Entity);
			Fragment2NPC->m_Entity = Fragment2Entity;
			Fragment2Entity->moving = true;
			Fragment2Entity->setNPC(Fragment2NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);


			// Fragment NPC3
			FragmentPos = { 830, 240 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment3Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment3.png"), 1, clips, 23);
			Entities.push_back(Fragment3Entity);
			std::vector<std::string> Fragment3Dialogue = { "you stayed silent, trying to get enough context to the conversation to join.", "You could not join. in this instant, you dont know anything before this."};
			std::shared_ptr<NPC> Fragment3NPC = std::make_shared<SIGNNPC>(Fragment3Dialogue, Fragment3Entity);
			Fragment3NPC->m_Entity = Fragment3Entity;
			Fragment3Entity->setNPC(Fragment3NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);


			// Fragment NPC4
			FragmentPos = { 130, 240 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment4Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment4.png"), 1, clips, 24);
			Entities.push_back(Fragment4Entity);
			std::vector<std::string> Fragment4Dialogue = { "The deeper we went the more time had stopped existing. Time was emulated.", " For time did not exist in this void of unrendered thoughts... and judgement of what we confirmed existed."};
			std::shared_ptr<NPC> Fragment4NPC = std::make_shared<SIGNNPC>(Fragment4Dialogue, Fragment4Entity);
			Fragment4NPC->m_Entity = Fragment4Entity;
			Fragment4Entity->setNPC(Fragment4NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);

			// Fragment NPC5
			FragmentPos = { 580, 150 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment5Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment5.png"), 1, clips, 25);
			Entities.push_back(Fragment5Entity);
			std::vector<std::string> Fragment5Dialogue = { "We had no bodies. We were faces. Each face ended with increased transparency over distance from the nose.", " Each face was connected to the ones to each side of it.", " In this semi-circle given the third axis. Like a sphere cut in half.", " I knew not who loomed above. But I felt them at the edge of my perception." };
			std::shared_ptr<NPC> Fragment5NPC = std::make_shared<SIGNNPC>(Fragment5Dialogue, Fragment5Entity);
			Fragment5NPC->m_Entity = Fragment5Entity;
			Fragment5Entity->setNPC(Fragment5NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);


			// Fragment NPC6
			FragmentPos = { 280, 150 };
			FragmentRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			FragmentCB = { int(FragmentPos.x), int(FragmentPos.y), 200, 200 };
			auto Fragment6Entity = std::make_shared<Entity>(FragmentPos, FragmentCB, FragmentRect, getTexture("data/fragment6.png"), 1, clips, 26);
			Entities.push_back(Fragment6Entity);
			std::vector<std::string> Fragment6Dialogue = { "I learned the topic of the conversation through context clues and tone.", "I asked no questions because even though i knew nothing, my previous self was not connected to what I was.", "Instead, it was in the middle of the group. We were surrounding who I was.", " We were discussing my actions. My thoughts. My perspective. Everything was exposed. ", " I felt no shame because again, I was detached from who I was. Yet connected from previous association.", " We saw everything. My impulses and ignorance. My lies. We saw how weak I really was." };
			std::shared_ptr<NPC> Fragment6NPC = std::make_shared<SIGNNPC>(Fragment6Dialogue, Fragment6Entity);
			Fragment6NPC->m_Entity = Fragment6Entity;
			Fragment6Entity->setNPC(Fragment6NPC);
			//collisionBoxes.push_back(&Fragment1Entity->m_Collider);

		}
		break;

	case 5:
		if (!Map->loadFromFile("data/sewertest.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.5f;
						// Exit Puddle
			Vector2f PuddlePos(500, 500);
			SDL_Rect puddleRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 0,200,200, 200 });
			clips.push_back({ 0,200 * 2,200, 200 });
			clips.push_back({ 200,0,200, 200 });
			clips.push_back({ 200,200,200, 200 });
			clips.push_back({ 200,200 * 2,200, 200 });
			clips.push_back({ 200 * 2,0,200, 200 });
			clips.push_back({ 200 * 2,200,200, 200 });
			clips.push_back({ 200 * 2,200 * 2,200, 200 });
			SDL_Rect puddleCB = { PuddlePos.x, PuddlePos.y, 200, 200 };
			auto PuddleEntity = std::make_shared<Entity>(PuddlePos, puddleCB, puddleRect, getTexture("data/Puddle.png"), 7, clips, 69);
			Entities.push_back(PuddleEntity);
			Vector2f PuddleOutPos(400, 200);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "test", PuddleOutPos);
			PuddleNPC->m_Entity = PuddleEntity;
			PuddleEntity->moving = true;
			PuddleEntity->setNPC(PuddleNPC);
			collisionBoxes.push_back(&PuddleEntity->m_Collider);
		}

		break;

	default:
		if (!Map->loadFromFile("data/Error.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		break;
	}
	leveldimentions.x = Map->getWidth();
	leveldimentions.y = Map->getHeight();
	gameState.levelWidth = Map->getWidth();
	gameState.levelHeight = Map->getHeight();
	printf("LoadLevel: Map dimensions = %d x %d\n", Map->getWidth(), Map->getHeight());
	camera.mapWidth = Map->getWidth();
	camera.mapHeight = Map->getHeight();
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

//void PopulateInventory(std::vector<int> itemIDs) {
//	for (int id : itemIDs) {
//		std::shared_ptr<Item> newItem;
//		switch (id) {
//		case 0:
//			printf("Loaded Healing Item into inventory.\n");
//			newItem = std::make_shared<BandAid>();
//			break;
//		case 1:
//			printf("Loaded Key Item into inventory.\n");
//			newItem = std::make_shared<Key>();
//			break;
//		default:
//			printf("Unknown item ID %d in save data.\n", id);
//			newItem = std::make_shared<Item>();
//			break;
//		}
//		
//		gameState.Inventory.push_back(newItem);
//	}
//}

void SaveInventory(std::vector<int>& outItemIDs) {
	outItemIDs.clear();
	for (const auto& item : gameState.Inventory) {
		outItemIDs.push_back(item->m_ItemID);
	}
}	
