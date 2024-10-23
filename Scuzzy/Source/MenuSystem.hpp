#ifndef MENUSYSTEM_HPP
#define MENUSYSTEM_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

// Menu states
enum MenuState {
    MAIN_MENU,
    INVENTORY_MENU,
    ITEM_OPTIONS_MENU,
    STATS_MENU
};

// Declare your current menu state globally, if needed
extern MenuState currentMenu;
//extern int selectedIndex;

// Function declarations for rendering and input handling
void MS_renderMenu(SDL_Renderer* renderer, TTF_Font* font);
void MS_handleMenuInput(SDL_Event event);

// Specific menu handling functions
void handleMainMenuSelection(SDL_Event event);
void handleInventoryMenuSelection(SDL_Event event);
void handleItemOptionsMenuSelection(SDL_Event event);
void handleStatsMenu(SDL_Event event);

void renderMainMenu(SDL_Renderer* renderer, TTF_Font* font);
void renderInventoryMenu(SDL_Renderer* renderer, TTF_Font* font);
void renderItemOptionsMenu(SDL_Renderer* renderer, TTF_Font* font);
void renderStatsMenu(SDL_Renderer* renderer, TTF_Font* font);

#endif
