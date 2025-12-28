#pragma once
#ifndef ITEM_H
#define ITEM_H

#include <string>

class GameState; // Forward declaration

/// <summary>
/// Manipulates the GameState Object based on ID. 
/// Caller is responsible for deleting the item from inventory.
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
    Key() {
        m_ItemID = 2;
        m_ItemName = "Key";
        m_ItemDescription = "A small key that unlocks doors.";
    }
    int Use() override;
};

#endif // ITEM_H