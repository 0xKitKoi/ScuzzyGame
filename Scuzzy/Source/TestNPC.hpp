#include "Source/Entity.hpp"
#include "Source/NPC.hpp"
#include "Source/Item.hpp"
#include "Source/MenuSystem.hpp"
#include <vector>
#include <string>

extern int MS_selectedIndex;

class DoorNPC : public NPC {
public:
    DoorNPC(std::shared_ptr<Entity> entity, std::string room, Vector2f Location) : NPC(entity, gameState.Text) { // needs a vector to shutup
        m_Location = Location;
        m_room = room;
    }
    //void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override { 
        if (m_checked) {
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


class MerchantNPC : public NPC {
public:
    struct ShopItem { int itemID; int price; std::string name; ShopItem(int id=0,int p=0,std::string n="") : itemID(id), price(p), name(n){} };

    MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity) : NPC(entity, dialogue) {}

    MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity, const std::vector<ShopItem>& stock)
        : NPC(entity, dialogue), m_Stock(stock) {}

    // When the player checks the merchant, open the shop menu
    void Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) override {
        m_Entity->moving = true;
        if (m_checked) {
            gameState.currentNPC = this; // expose ourselves to the menu system
            gameState.inMenu = true;
            currentMenu = SHOP_MENU;
            MS_selectedIndex = 0;
            m_checked = false;
        }
        /*
        // play animation in the sprite clippings:
        m_Entity->lastFrameTime += deltaT * 1000.0f;
		if (m_Entity->lastFrameTime >= m_Entity->frameDuration) {
			m_Entity->currentFrameCount = (m_Entity->currentFrameCount + 1) % m_Entity->FRAME_COUNT;
			m_Entity->lastFrameTime = 0;
		}
        SDL_Rect srcRect = m_Entity->m_Clips[m_Entity->currentFrameCount]; // render the sprite at index of animation

        m_Entity->getTex()->render(m_Entity->m_PosX, m_Entity->m_PosY, &srcRect);*/
	}
	
  

    // Check if player can afford item at index
    bool canAfford(size_t idx) const {
        if (idx >= m_Stock.size()) return false;
        return gameState.money >= m_Stock[idx].price;
    }

    // Attempt to purchase; returns true if purchased
    bool purchase(size_t idx) {
        if (idx >= m_Stock.size()) return false;
        const ShopItem &it = m_Stock[idx];
        if (gameState.money < it.price) return false;
        gameState.money -= it.price;

        // Create concrete item and add to inventory
        std::shared_ptr<Item> newItem;
        switch (it.itemID) {
        case 1:
            newItem = std::make_shared<BandAid>();
            break;
        case 2:
            newItem = std::make_shared<Key>();
            break;
        default:
            newItem = std::make_shared<Item>();
            break;
        }
        gameState.Inventory.push_back(newItem);
        return true;
    }

    std::vector<ShopItem> m_Stock;
};


/*
    // Create different types of NPCs
    VillagerNPC villager({"Hi!", "Need help?", "Bye"});
    MerchantNPC merchant({"Buy something?", "See my stock", "Leave"});

*/  