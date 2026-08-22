#include "Source/Helper.hpp"
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"
#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/NPC.hpp"

#include <string>
#include <vector>
#include "TestNPC.hpp" // i didnt know u can include a cpp file 
#include "Source/Enemies.hpp"
#include "Source/GameState.hpp"
#include "Source/Item.hpp"
#include "Source/ItemRegistry.hpp"

#include <random>

extern std::vector<std::shared_ptr<Entity>> Entities;
extern std::vector<SDL_Rect*> collisionBoxes;
extern std::vector<SDL_Rect> staticCollisionBoxes;
extern std::vector<SDL_Rect> clips;

extern Mix_Chunk* gExplosionSound;
extern Mix_Chunk* gAwHellNawSound;

extern Camera camera;

extern std::shared_ptr<LTexture> getTexture(const std::string& filename);

int LevelIDFromName(std::string name) {
	if (name == "test") { return 0; }
	else if (name == "DarkAlley") { return 1; }
	else if (name == "Level2") { return 2; }
	else if (name == "MLEM") { return 3; }
	else if (name == "MAGICANT") { return 4; }
	else if (name == "NOOMSIDE") { return 5; }
	else if (name == "FORGOTTENCAVE") { return 6; }
	else if (name == "StartingAlley") { return 7; }
	else if (name == "NoomSideCafe") { return 8; }
	else if (name == "NoomSideCafe2") { return 9; }
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
			//std::vector<MerchantNPC::ShopItem> stock = {MerchantNPC::ShopItem{2, 2, "test"}, MerchantNPC::ShopItem{1, 2, "weenie"} };
			//std::vector<MerchantNPC::ShopItem> stock = {
			//	MerchantNPC::ShopItem{1, 5},
			//	MerchantNPC::ShopItem{2222, 5},
			//};
			std::vector<MerchantNPC::ShopItem> stock = {
				MerchantNPC::ShopItem {1, 5, "Weenie Key", 81, "test"}
			};
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
			Vector2f outpos(300, 200);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity, "DarkAlley", outpos, 81); // DOOR ID 81 added. important for keys!
			
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

			gameState.mapScaling = 1.0f;


			std::vector<SDL_Rect> boundaryBoxes = {
				{ 226, 30, 177, 96 },
				{ 406, 114, 69, 80 },
				{ 420, 195, 48, 27 },
				{ 433, 225, 46, 48 },
				{ 443, 276, 50, 39 },
				{ 345, 326, 139, 107 },
				{ 369, 310, 84, 16 },
				{ 363, 436, 122, 27 },
				{ 417, 466, 93, 166 },
				{ 145, 73, 63, 373 },
				{ 80, 393, 63, 61 },
				{ 83, 451, 51, 32 },
				{ 88, 491, 39, 27 },
				{ 95, 520, 23, 24 },
				{ 99, 558, 59, 33 },
				{ 97, 597, 57, 156 },
				{ 80, 738, 58, 284 },
				{ 522, 634, 84, 71 },
				{ 544, 701, 105, 116 },
				{ 570, 822, 84, 183 },
				{ 136, 988, 435, 9 },
			};
						collisionBoxes.clear();
			collisionBoxes.reserve(boundaryBoxes.size()); // Optimizes memory allocation

			for (auto& r : boundaryBoxes) {
				collisionBoxes.push_back(new SDL_Rect(r));
			}


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
			Vector2f outpos(1000, 222); // (240, 300);//(1000, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity2, "NOOMSIDE", outpos);
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
			Vector2f PuddleOutPos(300, 200);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "DarkAlley", PuddleOutPos);
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
		if (!Map->loadFromFile("data/noomside.png")) // "data/sewertest.png"
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.0f;



			std::vector<SDL_Rect> boundaryBoxes = {
				{ 4551, 0, 16, 8 },
				{ 4543, 14, 15, 11 },
				{ 4517, 31, 30, 9 },
				{ 4491, 46, 30, 21 },
				{ 4468, 73, 27, 20 },
				{ 4448, 82, 25, 28 },
				{ 4427, 101, 24, 31 },
				{ 4412, 122, 24, 34 },
				{ 4393, 146, 25, 26 },
				{ 4374, 164, 25, 28 },
				{ 4355, 179, 27, 31 },
				{ 4349, 197, 17, 33 },
				{ 4320, 212, 32, 35 },
				{ 4290, 233, 38, 33 },
				{ 4268, 258, 31, 35 },
				{ 4244, 283, 34, 32 },
				{ 4214, 297, 47, 40 },
				{ 4181, 330, 40, 42 },
				{ 4160, 366, 45, 33 },
				{ 4013, 373, 134, 94 },
				{ 3859, 321, 104, 85 },
				{ 3933, 395, 84, 54 },
				{ 3790, 340, 59, 32 },
				{ 3834, 372, 24, 19 },
				{ 3725, 304, 51, 36 },
				{ 3764, 338, 24, 22 },
				{ 3622, 256, 66, 44 },
				{ 3672, 299, 44, 24 },
				{ 3538, 233, 51, 33 },
				{ 3575, 269, 45, 22 },
				{ 3473, 215, 49, 27 },
				{ 3509, 238, 22, 22 },
				{ 3411, 188, 45, 27 },
				{ 3443, 213, 30, 18 },
				{ 3342, 156, 37, 30 },
				{ 3358, 171, 49, 24 },
				{ 3390, 186, 20, 23 },
				{ 2874, 81, 60, 31 },
				{ 2928, 90, 65, 33 },
				{ 2977, 100, 71, 31 },
				{ 3033, 103, 84, 36 },
				{ 3093, 114, 84, 37 },
				{ 3161, 129, 82, 32 },
				{ 3239, 136, 71, 34 },
				{ 3293, 149, 62, 31 },
				{ 2788, 57, 87, 43 },
				{ 2681, 43, 103, 29 },
				{ 2748, 66, 44, 22 },
				{ 2606, 21, 73, 36 },
				{ 2513, 8, 88, 30 },
				{ 2414, 10, 100, 12 },
				{ 2326, 10, 88, 10 },
				{ 2295, 19, 41, 24 },
				{ 2275, 43, 31, 27 },
				{ 2254, 69, 25, 25 },
				{ 2226, 96, 30, 26 },
				{ 2197, 121, 32, 25 },
				{ 2166, 139, 30, 38 },
				{ 2138, 176, 30, 29 },
				{ 2106, 205, 31, 35 },
				{ 2076, 237, 53, 59 },
				{ 2087, 294, 48, 33 },
				{ 2270, 280, 72, 41 },
				{ 2342, 291, 70, 47 },
				{ 2412, 314, 49, 41 },
				{ 2463, 327, 55, 51 },
				{ 2516, 341, 68, 53 },
				{ 2585, 353, 67, 67 },
				{ 2650, 379, 89, 60 },
				{ 2733, 398, 52, 55 },
				{ 2787, 418, 77, 61 },
				{ 2965, 481, 766, 441 },
				{ 3045, 459, 592, 22 },
				{ 3092, 433, 482, 24 },
				{ 3162, 400, 324, 30 },
				{ 3212, 370, 176, 27 },
				{ 3243, 349, 87, 18 },
				{ 2748, 599, 48, 36 },
				{ 2790, 565, 73, 23 },
				{ 2687, 631, 69, 26 },
				{ 2618, 658, 101, 46 },
				{ 2543, 706, 77, 43 },
				{ 2454, 750, 91, 59 },
				{ 2362, 795, 97, 65 },
				{ 2166, 857, 200, 63 },
				{ 1971, 860, 192, 19 },
				{ 1920, 407, 283, 255 },
				{ 1958, 665, 165, 75 },
				{ 1995, 310, 93, 88 },
				{ 1814, 406, 109, 116 },
				{ 1723, 477, 90, 71 },
				{ 1664, 519, 66, 79 },
				{ 1602, 556, 51, 71 },
				{ 1525, 602, 75, 99 },
				{ 1593, 689, 62, 48 },
				{ 1630, 705, 60, 59 },
				{ 1652, 722, 81, 65 },
				{ 1691, 732, 81, 73 },
				{ 1739, 749, 81, 73 },
				{ 1781, 767, 91, 79 },
				{ 1841, 783, 83, 91 },
				{ 1883, 806, 99, 96 },
				{ 3300, 1029, 137, 146 },
				{ 3011, 930, 107, 36 },
				{ 3072, 968, 52, 41 },
				{ 3113, 1002, 65, 42 },
				{ 3164, 1028, 68, 58 },
				{ 3206, 1051, 73, 61 },
				{ 3247, 1085, 74, 48 },
				{ 2935, 925, 68, 571 },
				{ 2857, 1497, 92, 72 },
				{ 2770, 1628, 56, 50 },
				{ 2825, 1616, 40, 28 },
				{ 2861, 1572, 52, 28 },
				{ 2675, 1708, 63, 71 },
				{ 2742, 1661, 44, 53 },
				{ 2600, 1768, 63, 83 },
				{ 2480, 1865, 82, 78 },
				{ 2552, 1826, 64, 59 },
				{ 2642, 1755, 51, 46 },
				{ 2430, 1936, 67, 74 },
				{ 2357, 1975, 60, 197 },
				{ 2044, 1986, 54, 184 },
				{ 1986, 1966, 59, 85 },
				{ 1909, 1912, 73, 79 },
				{ 1850, 1868, 68, 83 },
				{ 1784, 1829, 82, 71 },
				{ 1700, 1776, 105, 68 },
				{ 1635, 1716, 95, 65 },
				{ 1542, 1670, 116, 53 },
				{ 1509, 1196, 105, 502 },
				{ 1511, 700, 109, 494 },
				{ 1428, 1066, 86, 22 },
				{ 1479, 1093, 37, 29 },
				{ 1338, 1023, 71, 226 },
				{ 1249, 1279, 66, 61 },
				{ 1303, 1249, 78, 30 },
				{ 1179, 1328, 80, 76 },
				{ 1080, 1375, 111, 76 },
				{ 946, 1154, 34, 219 },
				{ 976, 1335, 82, 38 },
				{ 1020, 1378, 70, 27 },
				{ 792, 1022, 28, 231 },
				{ 827, 1042, 104, 125 },
				{ 885, 1147, 59, 50 },
				{ 969, 576, 45, 62 },
				{ 990, 608, 61, 58 },
				{ 1021, 642, 62, 58 },
				{ 1088, 486, 448, 236 },
				{ 1128, 442, 105, 48 },
				{ 1178, 383, 52, 67 },
				{ 1213, 345, 57, 97 },
				{ 1246, 311, 51, 83 },
				{ 1287, 264, 65, 94 },
				{ 1334, 211, 66, 95 },
				{ 1386, 148, 49, 123 },
				{ 1430, 93, 52, 93 },
				{ 1466, 44, 67, 81 },
				{ 1494, 1, 145, 56 },
				{ 1063, 2, 434, 53 },
				{ 978, 18, 83, 90 },
				{ 919, 99, 73, 65 },
				{ 858, 163, 61, 65 },
				{ 789, 196, 113, 39 },
				{ 702, 108, 120, 81 },
				{ 363, 62, 333, 404 },
				{ 571, 458, 111, 79 },
				{ 409, 463, 160, 77 },
				{ -24, 250, 385, 495 },
				{ 114, 743, 118, 151 },
				{ 229, 748, 116, 51 },
				{ 8, 748, 107, 78 },
				{ 276, 1289, 101, 57 },
				{ 184, 1244, 127, 71 },
				{ 129, 1205, 123, 75 },
				{ 82, 1175, 119, 54 },
				{ 17, 1116, 91, 68 },
				{ -5, 1062, 33, 69 },
				{ 338, 1317, 114, 585 },
				{ 279, 1910, 77, 86 },
				{ 333, 1894, 87, 53 },
				{ 215, 2002, 88, 70 },
				{ 264, 1969, 61, 70 },
				{ 140, 2054, 90, 78 },
				{ 75, 2115, 85, 85 },
				{ 11, 2172, 71, 93 },
				{ 848, 637, 100, 109 },
				{ 917, 607, 65, 60 },
				{ 795, 745, 94, 83 },
				{ 794, 825, 96, 199 },
				{ 5003, 716, 189, 32 },
				{ 4977, 725, 32, 27 },
				{ 4950, 751, 25, 33 },
				{ 4924, 789, 34, 36 },
				{ 4907, 829, 57, 368 },
				{ 4909, 1199, 60, 261 },
				{ 4883, 1377, 28, 43 },
				{ 4866, 1425, 36, 48 },
				{ 4837, 1477, 44, 44 },
				{ 4808, 1499, 54, 29 },
				{ 4785, 1520, 45, 18 },
				{ 4440, 1585, 248, 39 },
				{ 4618, 1559, 172, 27 },
				{ 4742, 1535, 85, 22 },
				{ 4397, 1603, 42, 304 },
				{ 4367, 1655, 30, 220 },
				{ 4806, 2028, 757, 34 },
				{ 5067, 2064, 484, 41 },
				{ 5193, 2107, 281, 37 },
				{ 4655, 1976, 914, 35 },
				{ 4440, 1893, 1099, 36 },
				{ 4525, 1935, 1031, 30 },
				{ 5491, 1644, 31, 246 },
				{ 5457, 1606, 44, 33 },
				{ 5434, 1569, 49, 43 },
				{ 5412, 1539, 54, 40 },
				{ 5394, 1224, 34, 355 },
				{ 5425, 1278, 36, 79 },
				{ 5449, 990, 36, 311 },
				{ 5486, 1052, 33, 195 },
				{ 5523, 1102, 23, 95 },
				{ 5420, 950, 41, 55 },
				{ 5379, 908, 50, 55 },
				{ 5341, 867, 58, 60 },
				{ 5302, 818, 53, 67 },
				{ 5248, 785, 70, 52 },
				{ 5159, 746, 112, 59 },
				{ 4391, 1137, 45, 278 },
				{ 4345, 1137, 50, 152 },
				{ 4439, 1139, 55, 164 },
				{ 5389, 18, 48, 294 },
				{ 5342, 13, 52, 161 },
				{ 5443, 24, 47, 163 },
				{ 5689, 574, 42, 292 },
				{ 5743, 591, 50, 149 },
				{ 5631, 579, 56, 154 },
				{ 5600, 1338, 60, 292 },
				{ 5537, 1345, 185, 153 },
				{ 4769, 2558, 35, 291 },
				{ 4702, 2569, 183, 152 },
				{ 4402, 2369, 60, 124 },
				{ 4356, 2205, 170, 161 },
				{ 4128, 2196, 61, 124 },
				{ 4078, 2031, 175, 160 },
				{ 3856, 2384, 46, 128 },
				{ 3807, 2215, 177, 166 },
				{ 3940, 2743, 62, 120 },
				{ 3883, 2572, 184, 161 },
				{ 4091, 2939, 179, 58 },
				{ 4337, 2676, 124, 158 },
				{ 4450, 2631, 79, 177 },
				{ 4533, 2698, 44, 80 },
				{ 4288, 2694, 68, 59 },
				{ 3400, 2410, 77, 118 },
				{ 3339, 2233, 212, 171 },
				{ 3114, 2745, 99, 124 },
				{ 3051, 2583, 210, 160 },
				{ 2101, 1933, 257, 68 },
				{ 946, 2314, 157, 120 },
				{ 1105, 2365, 50, 98 },
				{ 1137, 2412, 73, 115 },
				{ 1215, 2456, 45, 136 },
				{ 1251, 2487, 50, 107 },
				{ 1296, 2523, 47, 109 },
				{ 1328, 2553, 43, 106 },
				{ 1346, 2587, 65, 95 },
				{ 1371, 2620, 74, 96 },
				{ 1430, 2660, 69, 107 },
				{ 1495, 2701, 47, 97 },
				{ 1522, 2732, 64, 108 },
				{ 1579, 2766, 46, 116 },
				{ 1609, 2810, 58, 93 },
				{ 1665, 2837, 37, 115 },
				{ 1700, 2866, 43, 109 },
				{ 1734, 2909, 61, 81 },
				{ 1782, 2953, 64, 41 },
				{ 871, 2357, 76, 112 },
				{ 800, 2455, 77, 88 },
				{ 727, 2527, 91, 98 },
				{ 682, 2595, 75, 104 },
				{ 617, 2660, 80, 112 },
				{ 541, 2732, 89, 128 },
				{ 479, 2805, 93, 110 },
				{ 422, 2865, 103, 125 },
				{ 366, 2924, 57, 63 },
				{ 748, 1128, 53, 67 },
				{ 706, 1023, 90, 143 },
				{ 764, 972, 31, 42 },
				{ 314, 471, 48, 275 },
				{ 3552, 916, 69, 53 },
				{ 3386, 921, 46, 100 },
				{ 2861, 457, 103, 125 },
				{ -1, 821, 13, 242 },
			};

									collisionBoxes.clear();
			collisionBoxes.reserve(boundaryBoxes.size()); // Optimizes memory allocation

			for (auto& r : boundaryBoxes) {
				collisionBoxes.push_back(new SDL_Rect(r));
			}

			// door ar 2510, 444 for roof
			// door at 2061, 1945 Boss building main entrance.
			// door at 784, 1107 cafe door
			// door at 4221, 2717 homeless tent
			// DOOR 3400, 960 // YELLOW building near boss building
			// 1000, 222 spawn
			// 100, 2570 // puddle placement.
			// 254, 740 tiny door.

			// squirle tree, family kicks u out


			// door at 784, 1107 cafe door
			Vector2f CafePuddlePos(784, 1107);
			SDL_Rect CafePuddleRect = { 0, 0, 200, 200 };
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
			SDL_Rect CafePuddleCB = { CafePuddlePos.x, CafePuddlePos.y, 200, 200 };
			auto CafePuddleEntity = std::make_shared<Entity>(CafePuddlePos, CafePuddleCB, CafePuddleRect, getTexture("data/Puddle.png"), 7, clips, 69);
			Entities.push_back(CafePuddleEntity);
			Vector2f CafePuddleOutPos(2400, 1400);
			std::shared_ptr<NPC> CafePuddleNPC = std::make_shared<DoorNPC>(CafePuddleEntity, "NoomSideCafe", CafePuddleOutPos);
			CafePuddleNPC->m_Entity = CafePuddleEntity;
			CafePuddleEntity->moving = true;
			CafePuddleEntity->setNPC(CafePuddleNPC);
			collisionBoxes.push_back(&CafePuddleEntity->m_Collider);





						// Exit Puddle
			Vector2f PuddlePos(100, 2570);
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
			Vector2f PuddleOutPos(240, 1630);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "forgottencave", PuddleOutPos);
			PuddleNPC->m_Entity = PuddleEntity;
			PuddleEntity->moving = true;
			PuddleEntity->setNPC(PuddleNPC);
			collisionBoxes.push_back(&PuddleEntity->m_Collider);



			// instant replay cutscene at 2160, 370
			/*
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
			*/
			Vector2f triggerPos(2160, 370);
			Vector2f wojackpos(triggerPos.x-500, triggerPos.y - 200);
			SDL_Rect triggerRect = { 0, 0, 300, 300 };
			SDL_Rect triggerCB = { triggerPos.x, triggerPos.y, 300, 300 };
			std::vector<SDL_Rect> instantReplayClips;
			instantReplayClips.push_back({ 0, 0, 0, 0 }); // no starting frame.
			instantReplayClips.push_back({ 0, 0, 1080, 967 }); // wojack meme
			std::vector<SDL_Rect> explClips;
			for (int i = 0; i < 15; i++) {
				explClips.push_back(SDL_Rect{ 72 * i, 0, 72, 100 });
			}
			auto triggerEntity = std::make_shared<Entity>(
				triggerPos, triggerCB, triggerRect,
				nullptr, //getTexture("data/RealisticExplosion72x100x18.png"), //nullptr,           // or a real texture 
				1,                 // framecount
				instantReplayClips,  // clips
				4003                // EntityID
			);
			Entities.push_back(triggerEntity);
			std::vector<std::unique_ptr<CutsceneAction>> cutsceneActions;
			cutsceneActions.push_back(std::make_unique<SoundEffectAction>(gAwHellNawSound, false, 0));
			cutsceneActions.push_back(std::make_unique<DialogueAction>(gameState, std::vector<std::string>{"wtf bro just clipped onto the roof did u fucking see that"}));
			std::vector<std::unique_ptr<CutsceneAction>> parallelActions;



			/*cutsceneActions*/ parallelActions.push_back(std::make_unique<SpriteShowAction>(getTexture("data/instantreplaywojack.png"), instantReplayClips, wojackpos, 2, 1000, true));
			//MovePlayerAction(Vector2f start, Vector2f target, float speed, std::vector<SDL_Rect> clips, Vector2f returnPosition = Vector2f(0.0f, 0.0f))
			//cutsceneActions.push_back(std::make_unique<MovePlayerAction>(Vector2f(gameState.player->m_PosX, gameState.player->m_PosY -100), Vector2f(gameState.player->m_PosX, gameState.player->m_PosY), 200.0f, gameState.player->DownWalking  /*std::vector<SDL_Rect>(gameState.player->DownWalking))*/ ) );
			float x = static_cast<float>(gameState.player->m_PosX);
			float y = static_cast<float>(gameState.player->m_PosY);

			/*cutsceneActions*/ parallelActions.push_back(
				std::make_unique<MovePlayerAction>(
					Vector2f(x-200.0f, y - 30.0f),
					Vector2f(x-200, y+100.0f),
					200.0f,
					gameState.player->DownWalking
				)
			);
			cutsceneActions.push_back(std::make_unique<ParallelAction>(std::move(parallelActions)));
			cutsceneActions.push_back(std::make_unique<ExplosionAction>(gExplosionSound, getTexture("data/RealisticExplosion72x100x18.png"), 15, explClips, triggerPos));
			
			//cutsceneActions.push_back(std::make_unique<HidePlayerAction>(100));
			//cutsceneActions.push_back(std::make_unique<SpriteShowAction>(getTexture("data/instantreplaywojack.png"), instantReplayClips, wojackpos, 2, 1000, true));
			//cutsceneActions.push_back(std::make_unique<MoveEntityAction>(gameState.player, Vector2f(wojackpos.x + 500, wojackpos.y + 200), 1.0f));
			
			std::shared_ptr<NPC> triggerNPC = std::make_shared<TriggerNPC>(triggerEntity, triggerCB, std::move(cutsceneActions));
			triggerEntity->setNPC(triggerNPC);



		}

		break;
	case 6:
		if (!Map->loadFromFile("data/forgottencave.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.0f;
			collisionBoxes.clear();
			std::vector<SDL_Rect> boundaryBoxes = {
				{ 851, 576, 90, 304 },
				{ 682, 509, 33, 11 },
				{ 657, 519, 48, 14 },
				{ 638, 532, 31, 20 },
				{ 627, 552, 37, 27 },
				{ 594, 578, 46, 54 },
				{ 582, 631, 30, 53 },
				{ 569, 684, 27, 39 },
				{ 578, 720, 12, 53 },
				{ 580, 773, 11, 51 },
				{ 583, 822, 22, 71 },
				{ 565, 885, 16, 13 },
				{ 557, 899, 20, 10 },
				{ 545, 909, 25, 25 },
				{ 531, 933, 29, 19 },
				{ 815, 713, 36, 12 },
				{ 834, 696, 25, 12 },
				{ 802, 733, 48, 69 },
				{ 779, 780, 27, 34 },
				{ 769, 813, 31, 13 },
				{ 740, 827, 42, 11 },
				{ 714, 838, 48, 11 },
				{ 714, 850, 30, 20 },
				{ 718, 871, 23, 23 },
				{ 724, 893, 30, 25 },
				{ 712, 917, 15, 76 },
				{ 695, 992, 15, 48 },
				{ 685, 1011, 11, 22 },
				{ 672, 1024, 41, 40 },
				{ 514, 959, 31, 17 },
				{ 502, 983, 28, 22 },
				{ 488, 1014, 27, 24 },
				{ 476, 1046, 16, 52 },
				{ 471, 1097, 17, 200 },
				{ 704, 1058, 138, 67 },
				{ 838, 1122, 19, 18 },
				{ 851, 1142, 26, 14 },
				{ 863, 1155, 34, 26 },
				{ 881, 1181, 57, 36 },
				{ 907, 1220, 68, 31 },
				{ 931, 1254, 74, 44 },
				{ 964, 1297, 75, 69 },
				{ 1007, 1369, 86, 74 },
				{ 493, 1293, 78, 68 },
				{ 574, 1330, 28, 26 },
				{ 600, 1358, 23, 32 },
				{ 610, 1391, 35, 38 },
				{ 640, 1424, 40, 39 },
				{ 672, 1459, 43, 50 },
				{ 697, 1485, 46, 57 },
				{ 1042, 1451, 251, 22 },
				{ 742, 1544, 427, 53 },
				{ 1058, 1605, 70, 22 },
				{ 994, 1635, 96, 22 },
				{ 899, 1663, 120, 21 },
				{ 851, 1694, 115, 24 },
				{ 756, 1731, 149, 17 },
				{ 699, 1754, 111, 23 },
				{ 482, 1702, 74, 27 },
				{ 448, 1627, 78, 75 },
				{ 529, 1598, 79, 60 },
				{ 616, 1595, 66, 71 },
				{ 665, 1668, 72, 26 },
				{ 712, 1701, 34, 48 },
				{ 632, 1752, 75, 14 },
				{ 334, 1653, 107, 38 },
				{ 253, 1633, 81, 35 },
				{ 179, 1598, 72, 174 },
				{ 257, 1741, 72, 65 },
				{ 324, 1755, 66, 57 },
				{ 388, 1783, 78, 33 },
				{ 471, 1805, 56, 47 },
				{ 529, 1837, 42, 48 },
				{ 579, 1871, 97, 55 },
				{ 692, 1890, 106, 64 },
				{ 823, 1920, 95, 53 },
				{ 939, 1946, 114, 36 },
				{ 1028, 1921, 86, 40 },
				{ 1076, 1898, 61, 24 },
				{ 1058, 1813, 14, 11 },
				{ 1076, 1820, 28, 11 },
				{ 1108, 1824, 47, 12 },
				{ 1159, 1835, 20, 16 },
				{ 1125, 1865, 47, 11 },
				{ 1149, 1857, 27, 9 },
				{ 1101, 1883, 40, 18 },
				{ 1078, 1807, 51, 7 },
				{ 1126, 1789, 65, 18 },
				{ 1176, 1761, 53, 25 },
				{ 1213, 1736, 66, 25 },
				{ 1260, 1704, 89, 35 },
				{ 1318, 1675, 82, 29 },
				{ 1389, 1645, 69, 38 },
				{ 1444, 1612, 63, 44 },
				{ 1490, 1584, 109, 45 },
				{ 1566, 1553, 68, 51 },
				{ 1628, 1521, 72, 40 },
				{ 1691, 1484, 99, 54 },
				{ 1777, 1437, 70, 62 },
				{ 1815, 1410, 100, 48 },
				{ 1897, 1186, 113, 239 },
				{ 1803, 1194, 88, 44 },
				{ 1734, 1240, 128, 20 },
				{ 1722, 1268, 71, 31 },
				{ 1666, 1300, 65, 22 },
				{ 1633, 1325, 59, 19 },
				{ 1604, 1353, 48, 22 },
				{ 1546, 1364, 54, 30 },
				{ 1509, 1392, 42, 28 },
				{ 1456, 1409, 52, 30 },
				{ 1427, 1435, 51, 21 },
				{ 1369, 1450, 63, 29 },
				{ 1274, 1469, 102, 36 },
				{ 807, 476, 61, 48 },
				{ 843, 527, 60, 45 },
				{ 678, 423, 38, 82 },
			};

			//collisionBoxes.clear();
			//collisionBoxes.reserve(boundaryBoxes.size());

			//std::transform(boundaryBoxes.begin(), boundaryBoxes.end(), std::back_inserter(collisionBoxes),
			//	[](SDL_Rect& r) { return &r; });
			collisionBoxes.clear();
			collisionBoxes.reserve(boundaryBoxes.size()); // Optimizes memory allocation

			for (auto& r : boundaryBoxes) {
				collisionBoxes.push_back(new SDL_Rect(r));
			}


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
			Vector2f outpos(500, 222); // (240, 300);//(1000, 960);
			std::shared_ptr<NPC> doornpc = std::make_shared<DoorNPC>(Doorentity2, "MAGICANT", outpos);
			doornpc->m_Entity = Doorentity2;
			Doorentity2->setNPC(doornpc);
			collisionBoxes.push_back(&Doorentity2->m_Collider);


		}
		break;


		case 7:
		if (!Map->loadFromFile("data/startingalley.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.0f;
			collisionBoxes.clear();
						std::vector<SDL_Rect> boundaryBoxes = {
				{ 226, 30, 177, 96 },
				{ 406, 114, 69, 80 },
				{ 420, 195, 48, 27 },
				{ 433, 225, 46, 48 },
				{ 443, 276, 50, 39 },
				{ 345, 326, 139, 107 },
				{ 369, 310, 84, 16 },
				{ 363, 436, 122, 27 },
				{ 417, 466, 93, 166 },
				{ 145, 73, 63, 373 },
				{ 80, 393, 63, 61 },
				{ 83, 451, 51, 32 },
				{ 88, 491, 39, 27 },
				{ 95, 520, 23, 24 },
				{ 99, 558, 59, 33 },
				{ 97, 597, 57, 156 },
				{ 80, 738, 58, 284 },
				{ 522, 634, 84, 71 },
				{ 544, 701, 105, 116 },
				{ 570, 822, 84, 183 },
				{ 136, 988, 435, 9 },
			};
						collisionBoxes.clear();
			collisionBoxes.reserve(boundaryBoxes.size()); // Optimizes memory allocation

			for (auto& r : boundaryBoxes) {
				collisionBoxes.push_back(new SDL_Rect(r));
			}


			// cutscene to trip out and go to forgotten cave then magicant, then the dark alley, then to noomside.
			
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
			Vector2f PuddleOutPos(240, 1630);
			std::shared_ptr<NPC> PuddleNPC = std::make_shared<DoorNPC>(PuddleEntity, "FORGOTTENCAVE", PuddleOutPos);
			PuddleNPC->m_Entity = PuddleEntity;
			PuddleEntity->moving = true;
			PuddleEntity->setNPC(PuddleNPC);
			collisionBoxes.push_back(&PuddleEntity->m_Collider);


		}
		break;

		case 8:
		if (!Map->loadFromFile("data/noomsidecade2.png")) //"data/CafeNoomSide.png"
		{
			printf("Failed to load sprite sheet texture!\n");
		}
		else {
			gameState.mapScaling = 1.0f;
			collisionBoxes.clear();
			std::vector<SDL_Rect> boundaryBoxes = {};


			// cafe cat pos: 1753, 868
			Vector2f CafeCatPOS = { 1753, 868 };
			SDL_Rect cafeCatRect = {0, 0, 128, 128} ;
			clips.clear();
			clips.push_back({0, 0, 128, 128});
			clips.push_back({128, 0, 128, 128});
			clips.push_back({128*2, 0, 128, 128});
			clips.push_back({128*3, 0, 128, 128});
			SDL_Rect cafeCatCollisionBox = {CafeCatPOS.x, CafeCatPOS.y, 200, 200};
			auto CafeCat = std::make_shared<Entity> (CafeCatPOS, cafeCatCollisionBox, cafeCatRect, 
			getTexture("data/cafecat-Sheet.png"),
			2,
			clips,
			024
			);
			Entities.push_back(CafeCat);	
			collisionBoxes.push_back(&CafeCat->m_Collider);
			std::vector<std::string> SHOPdialogue = {"meow meow meow MEOWWWWWW meow meow mree mreow meow meow mreow meow meow meow meow meow"};

			//std::vector<SpecialMerchantNPC::ShopItem> stock = {SpecialMerchantNPC::ShopItem{4, 5, "CatCafe Special", 10, "pupcup for kitty cats. catcup..? naw pupcup sounds better."}, SpecialMerchantNPC::ShopItem{4, 5, "CatCafe Flavored Band-Aid", 5, "A band-aid with a ... cat flavor. yeah."}, MerchantNPC::ShopItem{3, 1, "Catnip"}};
			std::vector<SpecialMerchantNPC::ShopItem> stock = {
				SpecialMerchantNPC::ShopItem{1001, 5}, // see ItemRegistry for more info on this.
				SpecialMerchantNPC::ShopItem{1002, 5},
				SpecialMerchantNPC::ShopItem{1003, 1}
			};

			std::shared_ptr<SpecialMerchantNPC> shopdood = std::make_shared<SpecialMerchantNPC>(
				SHOPdialogue, CafeCat, stock,
				std::vector<std::string>{"where am i", "what is this place?", "who are you?"}, // talkQuestions
				std::vector<std::string>{"Meow. (You are in NoomSide. We have a lake, rampant homicides, and some shops. We dont get many visitors. How'd YOU get here..?)", "Meow meow MEOW. (ur in my cat cafe. next door is the town's main attraction.)", "Meow meow meow. (im goobert. im a good boy.)" }  // talkAnswers
			);
			CafeCat->setNPC(shopdood);
			shopdood->m_Choices = { "Meow..?", "no."};
			shopdood->m_prompt = "Meow.";
			shopdood->m_CancelPrompt = "the cat spotted the duck. it's eyes widened.";

			CafeCat->AddAnimation("idle",      { {0,0,128,128} }, 180.0f, false);
			CafeCat->AddAnimation("surprised", { {128*3,0,128,128}, {128*4,0,128,120} }, 120.0f, false); // one-shot, holds last frame
			CafeCat->AddAnimation("happy",     { {0,0,128,128}, {128,0,128,128}}, 150.0f, true);

			CafeCat->PlayAnimation("idle");


			shopdood->SetReactionAnimation(MerchantReaction::Decline,   "surprised");
			shopdood->SetReactionAnimation(MerchantReaction::OfferTalk, "happy");
			shopdood->SetReactionAnimation(MerchantReaction::OfferBuy,  "happy");
			shopdood->SetReactionAnimation(MerchantReaction::TalkBack,  "happy");

			shopdood->SetTalkReactions({ "happy", "surprised" }); // one per talk question

			// --- SpecialMerchantNPC quick setup ---
			// 1. Entity->AddAnimation(name, frames, speed, loop);   // set up expressions once
			// 2. make_shared<SpecialMerchantNPC>(dialogue, entity, stock, questions, answers);
			// 3. SetReactionAnimation(MerchantReaction::X, "animName");  // per event, skip if none
			// 4. SetTalkReactions({ "anim1", "anim2", ... });             // optional, per question
			// 5. m_prompt / SetMainChoices({...}) / m_CancelPrompt to override default text




			// 2200, 925 secret locked door, code 89.
			Vector2f CafeSecretDoorPos(2200, 925);
			SDL_Rect CafeSecretDoorRect = { 0, 0, 128, 128 };
			clips.clear();
			clips.push_back({ 0, 0, 0, 0 });
			SDL_Rect CafeSecretDoorCollisionBox = { CafeSecretDoorPos.x, CafeSecretDoorPos.y, 128, 128 };
			std::shared_ptr<LTexture> noTexture = nullptr;
			auto CafeSecretDoorEntity = std::make_shared<Entity>(CafeSecretDoorPos, CafeSecretDoorCollisionBox, CafeSecretDoorRect, nullptr, 0, clips, 89);
			Entities.push_back(CafeSecretDoorEntity);
			collisionBoxes.push_back(&CafeSecretDoorEntity->m_Collider);
			auto CafeSecretDoorNPC = std::make_shared<DoorNPC>(CafeSecretDoorEntity, "MAGICANT", Vector2f(500, 222));
			CafeSecretDoorNPC->m_Entity = CafeSecretDoorEntity;
			CafeSecretDoorEntity->setNPC(CafeSecretDoorNPC);
			CafeSecretDoorNPC->m_Unlocked = false; // lock the door until the player gets the key item.
			CafeSecretDoorNPC->m_DoorID = 89; // set the door ID to match the key item ID.
			CafeSecretDoorNPC->m_prompt = "There is something here, but its locked. A kitty shaped key hole.";
			CafeSecretDoorNPC->m_Dialogue = { "The door is locked. It seems to require a special key." };


			// cafe duck pos: 1488, 1356
			Vector2f CafeDuckPOS = { 1488, 1430 };
			SDL_Rect cafeDuckRect = {0, 0, 64, 64} ;
			clips.clear();
			clips.push_back({0, 0, 64, 64});
			clips.push_back({64, 0, 64, 64});
			SDL_Rect cafeDuckCollisionBox = {CafeDuckPOS.x, CafeDuckPOS.y, 16, 16};
			auto CafeDuck = std::make_shared<Entity> (CafeDuckPOS, cafeDuckCollisionBox, cafeDuckRect, 
			getTexture("data/cafeduck-Sheet.png"),
			2,
			clips,
			025
			);
			CafeDuck->moving = true;
			Entities.push_back(CafeDuck);	
			collisionBoxes.push_back(&CafeDuck->m_Collider);

			// 2600, 1400 unlocked cafe exit door puddle.
			Vector2f CafeExitDoorPos(2600, 1400);
			SDL_Rect CafeExitDoorRect = { 0, 0, 200, 200 };
			clips.clear();
			clips.push_back({ 0, 0, 200, 200 });
			clips.push_back({ 200,0,200, 200 });
			clips.push_back({ 400,0,200, 200 });
			clips.push_back({ 0,200,200, 200 });
			SDL_Rect CafeExitDoorCollisionBox = { CafeExitDoorPos.x, CafeExitDoorPos.y, 200, 200 };
			auto CafeExitDoorEntity = std::make_shared<Entity>(CafeExitDoorPos, CafeExitDoorCollisionBox, CafeExitDoorRect, getTexture("data/Puddle.png"), 4, clips, 9001);
			Entities.push_back(CafeExitDoorEntity);
			collisionBoxes.push_back(&CafeExitDoorEntity->m_Collider);
			auto CafeExitDoorNPC = std::make_shared<DoorNPC>(CafeExitDoorEntity, "NOOMSIDE", Vector2f(740, 1300));
			CafeExitDoorNPC->m_Entity = CafeExitDoorEntity;
			CafeExitDoorEntity->setNPC(CafeExitDoorNPC);
			CafeExitDoorNPC->m_Unlocked = true; // unlock the door.
			//CafeExitDoorNPC->m_DoorID = 0; // set the door ID to 0.
			CafeExitDoorEntity->moving = true;


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




