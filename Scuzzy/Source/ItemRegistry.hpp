#pragma once
#ifndef ITEM_REGISTRY_H
#define ITEM_REGISTRY_H

#include "Item.hpp"
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>

class ItemRegistry {
public:
    struct Definition {
        std::string defaultName;
        std::string defaultDescription;
        // Builds the item given a param and a (possibly overridden) name/description
        std::function<std::shared_ptr<Item>(int param, const std::string& name, const std::string& desc)> create;
    };

    // The one place every item type gets registered. Add a new item type here, nowhere else.
    static const std::unordered_map<int, Definition> kDefinitions;

    // Main entry point. nameOverride/descOverride empty = use the registry defaults.
    static std::shared_ptr<Item> Create(int itemID, int param = 0,
                                         const std::string& nameOverride = "",
                                         const std::string& descOverride = "") {
        auto it = kDefinitions.find(itemID);
        if (it == kDefinitions.end()) {
            printf("\n [!] WARNING: ItemRegistry::Create() unknown itemID %d, giving generic Item", itemID);
            return std::make_shared<Item>();
        }
        const std::string& name = nameOverride.empty() ? it->second.defaultName : nameOverride;
        const std::string& desc = descOverride.empty() ? it->second.defaultDescription : descOverride;
        return it->second.create(param, name, desc);
    }

    static std::string GetDefaultName(int itemID) {
        auto it = kDefinitions.find(itemID);
        return it != kDefinitions.end() ? it->second.defaultName : "Unknown Item";
    }
};

inline const std::unordered_map<int, ItemRegistry::Definition>
ItemRegistry::kDefinitions = {

    { 1001, {
        "CatCafe Special",
        "pupcup for kitty cats. catcup..? naw pupcup sounds better.",
        [](int param, const std::string& name, const std::string& desc) {
            return std::make_shared<HealingItem>(
                1001,
                name,
                desc,
                10
            );
        }
    }},

    { 1002, {
        "CatCafe Flavored Band-Aid",
        "A band-aid with a ... cat flavor. yeah.",
        [](int param, const std::string& name, const std::string& desc) {
            return std::make_shared<HealingItem>(
                1002,
                name,
                desc,
                5
            );
        }
    }},

    { 2222, {
        "KittyCat Key",
        "Meow. (its a cat shaped key. You caught a glint of light in one of the merchant's cabinets.)",
        [](int param, const std::string& name, const std::string& desc) {
            return std::make_shared<Key>(param, name, desc);
        }
    }},

    { 1003, {
        "Catnip",
        "Kitty Kat Drugs.",
        [](int, const std::string& name, const std::string& desc) {
            return std::make_shared<Catnip>(name, desc);
        }
    }},
	{
		1,
		{
			"weenie key",
			"its a weird key thats made entirely out of hotdog.",
			[](int param, const std::string& name, const std::string& desc) {
				return std::make_shared<Key>(param, name, desc);
			}
		}
	}
	/*
	{
		ItemIDNumber, {
			"Default Name",
			"Default Description",
			[](int param, const std::string& name, const std::string& desc) {
				return std::make_shared<Item>(param, name, desc);
			}
		}
	}
	*/
};

#endif // ITEM_REGISTRY_H
