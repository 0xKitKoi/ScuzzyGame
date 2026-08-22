//#include "Source/Entity.hpp"
//#include "Source/NPC.hpp"
//#include "Source/Item.hpp"
#include "Source/MenuSystem.hpp"
#include "Source/CutSceneManager.hpp"
#include "Source/Enums.hpp"
#include "Source/ItemRegistry.hpp"
#include <functional>
#include <vector>
#include <string>

// forward declarations ONLY
class Entity;
class NPC;
class Item;

extern int MS_selectedIndex;

class Puddle : public NPC {
public:
	bool m_Unlocked = false;
	int m_PuddleID = -1;

	Puddle(std::shared_ptr<Entity> entity, std::string room, Vector2f Location, int PuddleID = -1) : NPC(entity, gameState.Text) { // needs a vector to shutup
		m_Location = Location;
		m_room = room;
		m_PuddleID = PuddleID;
	}
	//void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
	void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
		if (m_checked && m_Unlocked) {
			printf("Loading new room: %s\n", m_room.c_str());
			gameState.room = m_room;
			gameState.LoadingScreen = true; \
				gameState.DoneLoading = false;
			gameState.fade = true;
			gameState.textAvailable;
			gameState.callbackNPC = this;
			m_checked = false;
			gameState.player->SetPosX(m_Location.x);
			gameState.player->SetPosY(m_Location.y);
			gameState.player->reset({ m_Location.x, m_Location.y });
		}
		else if (!m_Unlocked && m_checked) {
			////////////////// TEST Puddle LOCKING MECHANIC //////////////////
			// this needs to be replaced with a mech to trigger Puddle unlocking.
			m_Unlocked = true;

            gameState.Text = { "You can't go through the puddle yet. There are actions you have not taken." };
            gameState.textIndex = 0;
            gameState.textAvailable = true;
            gameState.shouldAnimateText = true;
            gameState.textAnimating = true;
            gameState.textTimer = 0.0f;	
            gameState.currentCharIndex = 1; // start with first char pre-displayed to avoid empty-render crash
            gameState.currentDisplayText = !gameState.Text.empty() ? gameState.Text[0].substr(0, 1) : std::string();
			m_checked = false;

		}
	}
};


class DoorNPC : public NPC {
public:
	bool m_Unlocked = true;
	int m_DoorID = 0;

    DoorNPC(std::shared_ptr<Entity> entity, std::string room, Vector2f Location, int DoorID = -1) : NPC(entity, gameState.Text) { // needs a vector to shutup
        m_Location = Location;
        m_room = room;
		m_DoorID = DoorID;
    }
    //void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override { 
        if (m_checked && m_Unlocked) {
			//m_Entity->moving = true;
			//m_Entity->m_SpriteRect = m_Entity->m_Clips[1]; // this is the open door sprite
			//m_Entity->m_Texture->render(m_Entity->m_PosX, m_Entity->m_PosY, &m_Entity->m_Clips[1]); // render the open door sprite when the door is unlocked and checked. this is a quick and dirty way to show the door opening, but it works for now. ideally we would have an animation of the door opening, but that can be added later.
                    // Only update/render the door sprite if this door has a texture.
                if (m_Entity->m_Texture) {

                    if (m_Entity->m_Clips.size() > 1) {
                        m_Entity->m_SpriteRect = m_Entity->m_Clips[1];

                        m_Entity->m_Texture->render(
                            m_Entity->m_PosX,
                            m_Entity->m_PosY,
                            &m_Entity->m_Clips[1]
                        );
                    }
                }
			printf("Loading new room: %s\n", m_room.c_str());
            gameState.room = m_room;
			gameState.LoadingScreen = true;\
            gameState.DoneLoading = false;
            gameState.fade = true;
            gameState.textAvailable;
            gameState.callbackNPC = this;
            m_checked = false;
            gameState.player->SetPosX(m_Location.x);
			gameState.player->SetPosY(m_Location.y);
			gameState.player->reset({ m_Location.x, m_Location.y });
        }
		else if (!m_Unlocked && m_checked) {
			////////////////// TEST DOOR LOCKING MECHANIC //////////////////
			// this needs to be replaced with a mech to trigger door unlocking.
			//m_Unlocked = true;
			

			//gameState.Text = { "This Door is locked. There are actions you have not yet taken." };
            gameState.Text = m_prompt.empty() ? std::vector<std::string>{"This Door is locked. There are actions you have not yet taken."} : std::vector<std::string>{m_prompt};
            gameState.textIndex = 0;
            gameState.textAvailable = true;
            gameState.shouldAnimateText = true;
            gameState.textAnimating = true;
            gameState.textTimer = 0.0f;
            gameState.currentCharIndex = 1;
            gameState.currentDisplayText = !gameState.Text.empty() ? gameState.Text[0].substr(0, 1) : std::string();
			m_checked = false;

		}
    }
};

class VillagerNPC : public NPC {
public:
    VillagerNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity) : NPC(entity, dialogue) {}

    // Override handleChoice for Villager-specific behavior
    void handleChoice(int choice) override {
        if (choice == 0) {
            std::cout << "Basic ass villager waves hi." << std::endl;
        }
        else if (choice == 1) {
            std::cout << "You think I implemented quests yet???" << std::endl;
        }
        else {
            std::cout << "Oh, okay bye." << std::endl;
        }
    }
};

class SIGNNPC : public NPC {
public:
    SIGNNPC(const std::vector<std::string> dialogue, std::shared_ptr<Entity> entity) : NPC(entity, dialogue) {}
    //void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
        if (m_checked) {
            gameState.Text.clear();
            gameState.Text = m_Dialogue;
            gameState.textAvailable = true;
            m_checked = false;

            gameState.shouldAnimateText = true;
            gameState.textIndex = 0;
            gameState.currentCharIndex = 1; // offset because i need a char to start the animation.
            gameState.textTimer = 0.0f;
            gameState.textAnimating = true;
			if (!gameState.Text.empty())
				gameState.currentDisplayText = gameState.Text[0].substr(0, 1);
			else
				gameState.currentDisplayText = "";

        }
    }

    /*
    // Override handleChoice for Villager-specific behavior
    void handleChoice(int choice) override {
        if (choice == 0) {
            std::cout << "Basic ass villager waves hi." << std::endl;
        }
        else if (choice == 1) {
            std::cout << "You think I implemented quests yet???" << std::endl;
        }
        else {
            std::cout << "Oh, okay bye." << std::endl;
        }
    }
    */
};


// class MerchantNPC : public NPC {
// public:
//     std::string m_CancelPrompt;
//     //struct ShopItem { int itemID; int price; std::string name; ShopItem(int id=0,int p=0,std::string n="") : itemID(id), price(p), name(n){} };
//     struct ShopItem {
//         int itemID;
//         int price;
//         std::string name;
//         int param = 0; // generic extra data (e.g. door ID for keys); ignored by items that don't need it

//         ShopItem(int id = 0, int p = 0, std::string n = "", int extra = 0)
//             : itemID(id), price(p), name(n), param(extra) {}
//     };

//     MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity) : NPC(entity, dialogue) {}

//     MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity, const std::vector<ShopItem>& stock)
//         : NPC(entity, dialogue), m_Stock(stock) {
//         m_prompt = "hey kid, want a weenie ?";
//         m_Choices = {"damn straight I want a weenie", "...naw"};
//         m_CancelPrompt = "no weenie..? :(";
//     }

//     // When the player checks the merchant, open the shop menu
//     void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
//         m_Entity->moving = true;
//         if (m_checked) {
//             gameState.currentNPC = this; // expose ourselves to the menu system
// 			gameState.callbackNPC = this; // set callback for menu choices
//             gameState.inMenu = true;
//             //currentMenu = SHOP_MENU; // The MenuSystem variable is exposed, so the Merchant can manipulate it
// 			currentMenu = QUESTION_MENU;
//             MS_selectedIndex = 0;
//             m_checked = false;
//         }
//         /*
//         // play animation in the sprite clippings:
//         m_Entity->lastFrameTime += deltaT * 1000.0f;
// 		if (m_Entity->lastFrameTime >= m_Entity->frameDuration) {
// 			m_Entity->currentFrameCount = (m_Entity->currentFrameCount + 1) % m_Entity->FRAME_COUNT;
// 			m_Entity->lastFrameTime = 0;
// 		}
//         SDL_Rect srcRect = m_Entity->m_Clips[m_Entity->currentFrameCount]; // render the sprite at index of animation

//         m_Entity->getTex()->render(m_Entity->m_PosX, m_Entity->m_PosY, &srcRect);*/
// 	}

//     void handleChoice(int choice) override {
//         switch (choice) {
// 		case -1: // player pressed X to exit question prompt.
// 			currentMenu = MAIN_MENU;
// 			gameState.Text.push_back(m_CancelPrompt);
// 			gameState.textIndex = 0;
//             gameState.textAvailable = true;
// 			gameState.inMenu = false;
// 			//gameState.callbackNPC = nullptr;  // fuck ass bug
//             break;
//         case 0: // 0th index of options
//             //gameState.Text = { "WHAT??? NO!!! DONT TALK TO STRANGERS!!!" };
//             //gameState.textIndex = 0;
//             //gameState.textAvailable = true;
//             //gameState.inMenu = false;
// 			currentMenu = SHOP_MENU;
// 			break;
//         case 1: // 1st index of options
//             gameState.Text.clear();
//             gameState.Text.push_back(m_CancelPrompt);
//             gameState.textIndex = 0;
// 			gameState.textAvailable = true;
//             gameState.inMenu = false;
// 			/*gameState.callbackNPC = nullptr;*/
// 		default:
// 			printf("\n [!] ERROR: MerchantNPC::handleChoice() received invalid choice index: %d", choice);
//         }

// 	}
	
  

//     // Check if player can afford item at index
//     bool canAfford(size_t idx) const {
//         if (idx >= m_Stock.size()) return false;
//         return gameState.money >= m_Stock[idx].price;
//     }

//     // Attempt to purchase; returns true if purchased
//     bool purchase(size_t idx) {
//         if (idx >= m_Stock.size()) return false;
//         const ShopItem &it = m_Stock[idx];
//         if (gameState.money < it.price) return false;
//         gameState.money -= it.price;

//         // Create concrete item and add to inventory
//         std::shared_ptr<Item> newItem;
//         switch (it.itemID) {
//         case 1:
//             newItem = std::make_shared<BandAid>();
//             break;
//         case 2:
//             newItem = std::make_shared<Key>(81); // HEY! This is important. Keys now go to doors. DoorID here is 81. 
//             break;
//         default:
//             newItem = std::make_shared<Item>();
//             break;
//         }
//         gameState.Inventory.push_back(newItem);
//         return true;
//     }

//     std::vector<ShopItem> m_Stock;
// };


// // now that Merchant NPC is done, it is now a blueprint for custom merchants:

// class SpecialMerchantNPC : public MerchantNPC {
// public:
//     enum class Stage { NONE, GREETING, MAIN_CHOICE, TALK_SUBMENU, AWAIT_ANSWER_READ, SHOP };


//     SpecialMerchantNPC(const std::vector<std::string>& dialogue,
//                         std::shared_ptr<Entity> entity,
//                         const std::vector<ShopItem>& stock,
//                         std::vector<std::string> talkQuestions,
//                         std::vector<std::string> talkAnswers)
//         : MerchantNPC(dialogue, entity, stock),
//           m_TalkQuestions(std::move(talkQuestions)),
//           m_TalkAnswers(std::move(talkAnswers)) {
//         m_prompt   = "So, talk or business?";
//         m_MainChoices = {"Talk", "Buy"};
//         m_CancelPrompt = "Fine, be that way.";
//     }


//     // --- SpecialMerchantNPC quick setup ---
//     // 1. Entity->AddAnimation(name, frames, speed, loop);   // set up expressions once
//     // 2. make_shared<SpecialMerchantNPC>(dialogue, entity, stock, questions, answers);
//     // 3. SetReactionAnimation(MerchantReaction::X, "animName");  // per event, skip if none
//     // 4. SetTalkReactions({ "anim1", "anim2", ... });             // optional, per question
//     // 5. m_prompt / SetMainChoices({...}) / m_CancelPrompt to override default text

//     // Called once per merchant instance to configure its expressions
//     void SetReactionAnimation(MerchantReaction reaction, std::string animName) {
//         m_ReactionAnims[reaction] = std::move(animName);
//     }

//     // Per-question reaction, indexed same as m_TalkQuestions/m_TalkAnswers
//     void SetTalkReactions(std::vector<std::string> anims) {
//         m_TalkReactionAnims = std::move(anims);
//     }

//     void playReaction(MerchantReaction reaction) {
//         auto it = m_ReactionAnims.find(reaction);
//         if (it != m_ReactionAnims.end() && !it->second.empty()) {
//             m_Entity->PlayAnimation(it->second);
//         }
//         // if this merchant didn't configure that reaction, do nothing —
//         // it just keeps whatever animation was already playing.
//     }

//     void playTalkReaction(size_t idx) {
//         if (idx < m_TalkReactionAnims.size() && !m_TalkReactionAnims[idx].empty()) {
//             m_Entity->PlayAnimation(m_TalkReactionAnims[idx]);
//         }
//     }

//     void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
//         m_Entity->moving = true;

//         if (m_checked) {
//             gameState.Text = m_Dialogue; // greeting line(s)
//             gameState.textIndex = 0;
//             gameState.textAvailable = true;
//             gameState.inMenu = false;
//             m_stage = Stage::GREETING;
//             m_checked = false;
//         }

//         if (m_stage == Stage::GREETING && !gameState.textAvailable) {
//             openMainChoiceMenu();
//         }

//         if (m_stage == Stage::AWAIT_ANSWER_READ && !gameState.textAvailable) {
//             openTalkMenu();
//         }
//     }

//     void handleChoice(int choice) override {
//         switch (m_stage) {
//             case Stage::MAIN_CHOICE:  handleMainChoice(choice);  break;
//             case Stage::TALK_SUBMENU: handleTalkChoice(choice);  break;
//             default:
//                 printf("\n [!] ERROR: SpecialMerchantNPC::handleChoice() bad stage %d", (int)m_stage);
//         }
//     }

// private:
//     std::vector<std::string> m_TalkQuestions;
//     std::vector<std::string> m_TalkAnswers;
//     std::vector<std::string> m_MainChoices; // "Talk"/"Buy", kept separate from m_Choices
//     Stage m_stage = Stage::NONE;

//     std::unordered_map<MerchantReaction, std::string> m_ReactionAnims;
//     std::vector<std::string> m_TalkReactionAnims;

//     void openMainChoiceMenu() {
//         gameState.currentNPC = this;
//         gameState.callbackNPC = this;
//         gameState.inMenu = true;
//         m_Choices = m_MainChoices;
//         currentMenu = QUESTION_MENU;
//         MS_selectedIndex = 0;
//         m_stage = Stage::MAIN_CHOICE;
//     }

//     void handleMainChoice(int choice) {
//         switch (choice) {
//             case -1:
//                 playReaction(MerchantReaction::Decline);
//                 closeOut();
//                 break;
//             case 0:
//                 playReaction(MerchantReaction::OfferTalk);
//                 openTalkMenu();
//                 break;
//             case 1:
//                 playReaction(MerchantReaction::OfferBuy);
//                 currentMenu = SHOP_MENU;
//                 m_stage = Stage::SHOP;
//                 break;
//             default:
//                 printf("\n [!] ERROR: bad main choice %d", choice);
//         }
//     }

//     void openTalkMenu() {
//         gameState.currentNPC = this;
//         gameState.callbackNPC = this;
//         gameState.inMenu = true;
//         m_Choices = m_TalkQuestions; // TALK_MENU renders off m_Choices
//         currentMenu = TALK_MENU;
//         MS_selectedIndex = 0;
//         m_stage = Stage::TALK_SUBMENU;
//     }

// void handleTalkChoice(int choice) {
//         if (choice == -1) {
//             playReaction(MerchantReaction::TalkBack);
//             openMainChoiceMenu();
//             return;
//         }
//         if (choice < 0 || choice >= (int)m_TalkAnswers.size()) {
//             printf("\n [!] ERROR: bad talk choice %d", choice);
//             return;
//         }
//         playTalkReaction(choice);
//         gameState.Text = { m_TalkAnswers[choice] };
//         gameState.textIndex = 0;
//         gameState.textAvailable = true;
//         gameState.inMenu = false;
//         m_stage = Stage::AWAIT_ANSWER_READ;
//     }

//     void closeOut() {
//         currentMenu = MAIN_MENU;
//         gameState.Text.push_back(m_CancelPrompt);
//         gameState.textIndex = 0;
//         gameState.textAvailable = true;
//         gameState.inMenu = false;
//         gameState.callbackNPC = nullptr; // real exit, safe to clear here
//         m_stage = Stage::NONE;
//     }
// };

class MerchantNPC : public NPC {
public:

    // struct ShopItem {
    //     int itemID;
    //     int price;
    //     std::string name;
    //     int param = 0; // extra data, this holds  the door id for keys to point to a specific door.
    //     std::string description = ""; // optional override; empty = use item's default

    //     ShopItem(int id = 0, int p = 0, std::string n = "", int extra = 0, std::string desc = "")
    //         : itemID(id), price(p), name(n), param(extra), description(desc) {}
    // };
    //struct ShopItem {
    //    int itemID;
    //    int price;
    //    int param = 0;

    //    ShopItem(int id = 0, int p = 0, int extra = 0)
    //        : itemID(id), price(p), param(extra) {}
    //};

	struct ShopItem {
		int itemID;
		int price;
		int param = 0;
		std::string name;
		std::string description;

		// simple constructor (current usage)
		ShopItem(int id = 0, int p = 0, int extra = 0)
			: itemID(id), price(p), param(extra), name(), description() {
		}

		// compatibility constructor (restores previous API that took name/description)
		ShopItem(int id, int p, std::string n, int extra = 0, std::string desc = "")
			: itemID(id), price(p), param(extra), name(std::move(n)), description(std::move(desc)) {
		}
	};


    std::string m_CancelPrompt;

    MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity)
        : NPC(entity, dialogue) {}

    MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity, const std::vector<ShopItem>& stock)
        : NPC(entity, dialogue), m_Stock(stock) {
        m_prompt = "hey kid, want a weenie ?";
        m_Choices = {"damn straight I want a weenie", "...naw"};
        m_CancelPrompt = "no weenie..? :(";
    }

    // --- reaction system, usable by any MerchantNPC (base or derived) ---
    void SetReactionAnimation(MerchantReaction reaction, std::string animName) {
        m_ReactionAnims[reaction] = std::move(animName);
    }

    void playReaction(MerchantReaction reaction) {
        auto it = m_ReactionAnims.find(reaction);
        if (it != m_ReactionAnims.end() && !it->second.empty()) {
            m_Entity->PlayAnimation(it->second);
        }
    }

    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
        m_Entity->moving = true;
        if (m_checked) {
            gameState.currentNPC = this;
            gameState.callbackNPC = this;
            gameState.inMenu = true;
            currentMenu = QUESTION_MENU;
            MS_selectedIndex = 0;
            m_checked = false;
        }
    }

    void handleChoice(int choice) override {
        switch (choice) {
        case -1:
            currentMenu = MAIN_MENU;
            gameState.Text.push_back(m_CancelPrompt);
            gameState.textIndex = 0;
            gameState.textAvailable = true;
            gameState.inMenu = false;
            break;
        case 0:
            currentMenu = SHOP_MENU;
            break;
        case 1:
            gameState.Text.clear();
            gameState.Text.push_back(m_CancelPrompt);
            gameState.textIndex = 0;
            gameState.textAvailable = true;
            gameState.inMenu = false;
            break; // <-- fixed missing break from way back
        default:
            printf("\n [!] ERROR: MerchantNPC::handleChoice() received invalid choice index: %d", choice);
        }
    }

    bool canAfford(size_t idx) const {
        if (idx >= m_Stock.size()) return false;
        return gameState.money >= m_Stock[idx].price;
    }

    // bool purchase(size_t idx) {
    //     if (idx >= m_Stock.size()) return false;
    //     const ShopItem& it = m_Stock[idx];

    //     if (gameState.money < it.price) {
    //         playReaction(MerchantReaction::CantAfford);
    //         return false;
    //     }

    //     gameState.money -= it.price;

    //     std::shared_ptr<Item> newItem;
    //     auto factoryIt = kItemFactories.find(it.itemID);
    //     if (factoryIt != kItemFactories.end()) {
    //         newItem = factoryIt->second(it);
    //     } else {
    //         printf("\n [!] WARNING: purchase() unknown itemID %d, giving generic Item", it.itemID);
    //         newItem = std::make_shared<Item>();
    //     }

    //     gameState.Inventory.push_back(newItem);
    //     playReaction(MerchantReaction::PurchaseSuccess);
    //     return true;
    // }
    bool purchase(size_t idx) {
        if (idx >= m_Stock.size()) return false;

        const ShopItem& it = m_Stock[idx];

        if (gameState.money < it.price) {
            playReaction(MerchantReaction::CantAfford);
            return false;
        }

        gameState.money -= it.price;

        auto newItem = ItemRegistry::Create(it.itemID, it.param);

        gameState.Inventory.push_back(newItem);

        playReaction(MerchantReaction::PurchaseSuccess);
        return true;
    }

    std::vector<ShopItem> m_Stock;

private:
    // using ItemFactory = std::function<std::shared_ptr<Item>(const ShopItem&)>;
    // static const std::unordered_map<int, ItemFactory> kItemFactories;

    std::unordered_map<MerchantReaction, std::string> m_ReactionAnims;
};


// this is a global static map of itemID -> factory function, used by all MerchantNPCs to create items on purchase.
// Each itemID corresponds to a lambda that takes a ShopItem and returns a shared_ptr<Item> of the appropriate type.
// This allows for easy extension: to add a new item type, just add a new entry here with the itemID and a lambda that constructs the item.
// items are reusable, like keys for example. You can customize the keys name and description in the ShopItem, and the factory will use those values when creating the Key object.
// inline const std::unordered_map<int, MerchantNPC::ItemFactory> MerchantNPC::kItemFactories = {
//     { 1, [](const ShopItem&)    { return std::make_shared<BandAid>(); } },
//     { 2222, [](const ShopItem& it) { // this one is a key template. 
//         std::string desc = it.description.empty() ? "A key that opens a matching door." : it.description;
//         return std::make_shared<Key>(it.param, it.name, desc);
//     } },
//     { 3, [](const ShopItem&)    { return std::make_shared<Catnip>(); } },
//     { 4, [](const ShopItem& it) { return std::make_shared<HealingItem>(it.name, it.description, it.param); } }, // healing item template. 
// };

class SpecialMerchantNPC : public MerchantNPC {
public:
    enum class Stage { NONE, GREETING, MAIN_CHOICE, TALK_SUBMENU, AWAIT_ANSWER_READ, SHOP };

    SpecialMerchantNPC(const std::vector<std::string>& dialogue,
                        std::shared_ptr<Entity> entity,
                        const std::vector<ShopItem>& stock,
                        std::vector<std::string> talkQuestions,
                        std::vector<std::string> talkAnswers)
        : MerchantNPC(dialogue, entity, stock),
          m_TalkQuestions(std::move(talkQuestions)),
          m_TalkAnswers(std::move(talkAnswers)) {
        m_prompt = "So, talk or business?";
        m_MainChoices = {"Talk", "Buy"};
        m_CancelPrompt = "Fine, be that way.";
    }

    void SetMainChoices(std::vector<std::string> choices) { m_MainChoices = std::move(choices); }

    // Per-question reaction, indexed same as m_TalkQuestions/m_TalkAnswers
    void SetTalkReactions(std::vector<std::string> anims) {
        m_TalkReactionAnims = std::move(anims);
    }

    void playTalkReaction(size_t idx) {
        if (idx < m_TalkReactionAnims.size() && !m_TalkReactionAnims[idx].empty()) {
            m_Entity->PlayAnimation(m_TalkReactionAnims[idx]);
        }
    }

    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
        m_Entity->moving = true;

        if (m_checked) {
            gameState.Text = m_Dialogue;
            gameState.textIndex = 0;
            gameState.textAvailable = true;
            gameState.inMenu = false;
            m_stage = Stage::GREETING;
            m_checked = false;
        }

        if (m_stage == Stage::GREETING && !gameState.textAvailable) {
            openMainChoiceMenu();
        }

        if (m_stage == Stage::AWAIT_ANSWER_READ && !gameState.textAvailable) {
            openTalkMenu();
        }
    }

    void handleChoice(int choice) override {
        switch (m_stage) {
            case Stage::MAIN_CHOICE:  handleMainChoice(choice);  break;
            case Stage::TALK_SUBMENU: handleTalkChoice(choice);  break;
            default:
                printf("\n [!] ERROR: SpecialMerchantNPC::handleChoice() bad stage %d", (int)m_stage);
        }
    }

private:
    std::vector<std::string> m_TalkQuestions;
    std::vector<std::string> m_TalkAnswers;
    std::vector<std::string> m_MainChoices;
    std::vector<std::string> m_TalkReactionAnims;
    Stage m_stage = Stage::NONE;

    void openMainChoiceMenu() {
        gameState.currentNPC = this;
        gameState.callbackNPC = this;
        gameState.inMenu = true;
        m_Choices = m_MainChoices;
        currentMenu = QUESTION_MENU;
        MS_selectedIndex = 0;
        m_stage = Stage::MAIN_CHOICE;
    }

    void handleMainChoice(int choice) {
        switch (choice) {
            case -1:
                playReaction(MerchantReaction::Decline);
                closeOut();
                break;
            case 0:
                playReaction(MerchantReaction::OfferTalk);
                openTalkMenu();
                break;
            case 1:
                playReaction(MerchantReaction::OfferBuy);
                currentMenu = SHOP_MENU;
                m_stage = Stage::SHOP;
                break;
            default:
                printf("\n [!] ERROR: bad main choice %d", choice);
        }
    }

    void openTalkMenu() {
        gameState.currentNPC = this;
        gameState.callbackNPC = this;
        gameState.inMenu = true;
        m_Choices = m_TalkQuestions;
        currentMenu = TALK_MENU;
        MS_selectedIndex = 0;
        m_stage = Stage::TALK_SUBMENU;
    }

    void handleTalkChoice(int choice) {
        if (choice == -1) {
            playReaction(MerchantReaction::TalkBack);
            openMainChoiceMenu();
            return;
        }
        if (choice < 0 || choice >= (int)m_TalkAnswers.size()) {
            printf("\n [!] ERROR: bad talk choice %d", choice);
            return;
        }
        playTalkReaction(choice);
        gameState.Text = { m_TalkAnswers[choice] };
        gameState.textIndex = 0;
        gameState.textAvailable = true;
        gameState.shouldAnimateText = true;
        gameState.textAnimating = true;
        // This answer replaces the previous dialogue line, so its animation
        // must not inherit the previous line's cursor or timer.
        gameState.currentCharIndex = 0;
        gameState.textTimer = 0.0f;
        gameState.currentDisplayText.clear();
        gameState.inMenu = false;
        m_stage = Stage::AWAIT_ANSWER_READ;
    }

    void closeOut() {
        currentMenu = MAIN_MENU;
        gameState.Text.clear();
        gameState.Text.push_back(m_CancelPrompt);
        gameState.textIndex = 0;
        gameState.textAvailable = true;
        gameState.inMenu = false;
        gameState.callbackNPC = nullptr;
        m_stage = Stage::NONE;
    }
};







class TriggerNPC : public NPC {
public:

    SDL_Rect m_PovBox;
    bool fired = false;
    std::vector<std::unique_ptr<CutsceneAction>> m_Cutscenes;

    TriggerNPC(std::shared_ptr<Entity> entity, SDL_Rect povBox, std::vector<std::unique_ptr<CutsceneAction>> cutscenes) : NPC(entity, {}), m_PovBox(povBox) {

        m_Cutscenes = std::move(cutscenes);
    }
    //void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
        if (fired) return; // only trigger once. if you want it to trigger multiple times, remove this and make sure to reset the cutscene manager when the cutscene is done.
        // if player is in POV box, trigger the cutscene.
        if (SDL_HasIntersection(&m_PovBox, &PlayerPos)) {
            if (!gameState.inCutScene) { // only trigger if not already in a cutscene
                gameState.inCutScene = true;
                fired = true;
                gameState.cutsceneManager.m_IsActive = true;
                //gameState.cutsceneManager.m_Actions = m_Cutscenes; // set the cutscene actions to this trigger's cutscenes.

                //for (auto& action : m_Cutscenes) {
                //    gameState.cutsceneManager.AddAction(std::move(action)); // add the cutscene actions to the cutscene manager
                //}
                gameState.cutsceneManager.m_Actions = std::move(m_Cutscenes); // set the cutscene actions to this trigger's cutscenes.




                gameState.cutsceneManager.StartCutscene();
                
            }
        }
    }


};



// class ExplosionNPC : public NPC {
// public:
// bool fired = false;
//     ExplosionNPC(std::shared_ptr<Entity> entity) : NPC(entity, {}) {}

//     void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override  {
//         if (!fired) {
//             fired = true;
//             // play sound once. animate once. 
            
//         }
//         return;
        
//     }
// };


 class SoulRubberBandBallNPC : public NPC {
 public:
    bool fired = false;
    BackLayerDirection m_Direction = BackLayerDirection::TOP_LEFT;
    int m_BackFrameCountMax = 0; // how many frames this direction's set has
    std::vector<SDL_Rect> m_BackClipsTopLeft;
    std::vector<SDL_Rect> m_BackClipsTopRight;
    std::vector<SDL_Rect> m_BackClipsBottomLeft;
    std::vector<SDL_Rect> m_BackClipsBottomRight;

    // Returns whichever set matches the current direction.
    std::vector<SDL_Rect>& GetActiveBackClips() {
        switch (m_Direction) {
            case BackLayerDirection::TOP_LEFT:     return m_BackClipsTopLeft;
            case BackLayerDirection::TOP_RIGHT:    return m_BackClipsTopRight;
            case BackLayerDirection::BOTTOM_LEFT:  return m_BackClipsBottomLeft;
            case BackLayerDirection::BOTTOM_RIGHT: return m_BackClipsBottomRight;
        }
        return m_BackClipsTopLeft;
    }


    SoulRubberBandBallNPC(std::shared_ptr<Entity> entity) : NPC(entity, {}) {
        m_BackClipsTopLeft = std::vector<SDL_Rect>(m_Entity->m_Clips.begin() + 23, m_Entity->m_Clips.begin() + 30);
        m_BackClipsTopRight = std::vector<SDL_Rect>(m_Entity->m_Clips.begin() + 29, m_Entity->m_Clips.begin() + 36);
        m_BackClipsBottomLeft = std::vector<SDL_Rect>(m_Entity->m_Clips.begin() + 35, m_Entity->m_Clips.begin() + 42);
        m_BackClipsBottomRight = std::vector<SDL_Rect>(m_Entity->m_Clips.begin() + 41, m_Entity->m_Clips.begin() + 48);

    }

     void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override  {
         if (!fired) {
             fired = true;
              //play sound once. animate once. 

         }
         return;

     }
 };
