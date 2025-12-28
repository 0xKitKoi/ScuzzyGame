#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include "Math.hpp"
#include "LTexture.hpp"

int LevelIDFromName(std::string name);
Vector2f LoadLevel(std::string Room, LTexture* Map);
int UseItem(int ID);
std::string GetItemnameFromIndex(int index);
std::string GetItemDescription(int ID);
void SaveInventory(std::vector<int>& outItemIDs);
void PopulateInventory(std::vector<int> itemIDs);