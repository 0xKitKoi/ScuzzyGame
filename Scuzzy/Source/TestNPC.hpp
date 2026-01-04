#include "Source/Entity.hpp"
#include "Source/NPC.hpp"
#include <vector>

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
    MerchantNPC(const std::vector<std::string>& dialogue, std::shared_ptr<Entity> entity) : NPC(entity, dialogue) {}

    // Override handleChoice for Merchant-specific behavior
    void handleChoice(int choice) override {
        if (choice == 0) {
            std::cout << "I have wares if u have the coin." << std::endl;
        }
        else if (choice == 1) {
            std::cout << "Come back whe ur a little..hmm..richer!" << std::endl;
        }
        else {
            std::cout << "come back when items are a thing." << std::endl;
        }
    }
};


/*
    // Create different types of NPCs
    VillagerNPC villager({"Hi!", "Need help?", "Bye"});
    MerchantNPC merchant({"Buy something?", "See my stock", "Leave"});

*/  