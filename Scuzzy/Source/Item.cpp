#include "Source/Item.hpp"
#include "Source/GameState.hpp"

int BandAid::Use() {
    gameState.HP += 5;
    return 1;
}

int Key::Use() {
    gameState.money += 10;
    return 1;
}