#pragma once
#ifndef ITEM_H
#define ITEM_H

#include <string>
//#include "Source/GameState.hpp"
//#include "TestNPC.hpp"

class GameState; // Forward declaration

/// <summary>
/// Items Can affect the GameState when used.
/// Caller is responsible for deleting the item from inventory. This should happen in the Menu System. 
/// any new items that can be sold should be added to the merchant class's item factory for creation: static const std::unordered_map<int, ItemFactory> kItemFactories
/// </summary>
class Item {
public:
    int m_ItemID = 0;
    std::string m_ItemName = "Default Item Name";
    std::string m_ItemDescription = "Default Item Description";
    virtual int Use() { 
        printf("Using base Item class. This should be overridden.\n"); 
        return 0; 
    }
    virtual ~Item() = default; // Virtual destructor is important!
};

// FULL class definitions (not just forward declarations)
class BandAid : public Item {
public:
    BandAid() {
        m_ItemID = 1;
        m_ItemName = "Band-Aid";
        m_ItemDescription = "A simple band-aid that heals 5 HP.";
    }
    int Use() override;
};

class Key : public Item {
public:
    Key(int DoorID, std::string name = "Key", std::string description = "A key that opens a matching door.") {
        m_DoorID = DoorID;
        m_ItemID = 2222;
        m_ItemName = std::move(name);
        m_ItemDescription = std::move(description);
    }
    int m_DoorID = 0;
    int Use() override;
};

class Catnip : public Item {
public:
    Catnip(
        std::string name = "Catnip",
        std::string description = "Kitty Kat Drugs."
    ) {
        m_ItemID = 3001;
        m_ItemName = std::move(name);
        m_ItemDescription = std::move(description);
    }

    int Use() override;
};

class HealingItem : public Item {
public:
    HealingItem(int id, std::string name, std::string description, int healAmount) : m_HealAmount(healAmount) {
        m_ItemID = id; // Unique ID for HealingItem. 
        m_ItemName = name;
        m_ItemDescription = description;
    }
    int Use() override;
    int m_HealAmount = 0;
};


#endif // ITEM_H
