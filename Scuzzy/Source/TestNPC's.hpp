#include "Source/Entity.hpp"
#include "Source/NPC.hpp"
#include <vector>

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
    void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) override {
        if (m_checked) {
            gameState.Text = m_Dialogue;
            gameState.textAvailable = true;
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