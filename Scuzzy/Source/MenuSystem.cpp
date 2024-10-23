

#include "Source/MenuSystem.hpp"
#include "Source/GameState.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include < stdio.h >

//extern const int SCREEN_WIDTH;
//extern const int SCREEN_HEIGHT;

// Define the current menu state and selected index
MenuState currentMenu = MAIN_MENU;
int MS_selectedIndex = 0;
int subMenuSelectionIndex = 0;
std::vector<std::string> MAIN_MENU_Options = { "Check", "Items", "Stats" };


std::string GetItemnameFromIndex(int index) {
    switch (index) {
    case 1:
        return "Test Item 1";
        break;
    case 2:
        return "Test Item 2";
        break;
    default:
        printf("\n [!] ERROR: Could not get Item name at selection index: %d", index);
        return "ERROR";
        break;
    }
}


void MS_renderTextBox(SDL_Renderer* renderer) {

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    int boxWidth = screenWidth * 0.9;  // 90% of the screen width
    int boxHeight = 300;               // Fixed height for the text box
    int xPos = (screenWidth - boxWidth) / 2;  // Center the box horizontally
    int yPos = screenHeight - boxHeight - 20; // Place the box 20 pixels above the bottom

    // Set the color for the text box (e.g., black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect textBoxRect = { xPos, yPos, boxWidth, boxHeight };
    SDL_RenderFillRect(renderer, &textBoxRect);

    // Draw a red border around the text box
    SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &textBoxRect);
}

void MS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderMenuGrid(SDL_Renderer* renderer, TTF_Font* font, std::vector<std::string>* options) {
    // Get screen dimensions
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    // Render the text box at the bottom of the screen
    MS_renderTextBox(renderer);

    // Calculate text rendering position
    int boxWidth = screenWidth * 0.9;
    int xOffset = screenWidth * 0.05 + 10;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box

    int numOptions = std::min(static_cast<int>(options->size()), 12); // Cap to 12 options

    // Calculate grid dimensions
    const int maxColumns = 4;
    const int maxRows = (numOptions + maxColumns - 1) / maxColumns; // Calculate necessary rows
    int optionWidth = (boxWidth - (maxColumns + 1) * 10) / maxColumns; // 10 is spacing
    int optionHeight = 50; // Set a fixed height for each option

    SDL_Color white = { 255, 255, 255 };  // Normal text color
    SDL_Color red = { 237, 28, 36 };       // Highlighted text color

    for (int i = 0; i < numOptions; i++) {
        int column = i % maxColumns;  // Current column (0, 1, 2, 3)
        int row = i / maxColumns;      // Current row

        int currentX = xOffset + (column * (optionWidth + 10)); // 10 is the spacing
        int currentY = yOffset + (row * (optionHeight + 10));   // 10 is the spacing

        // Determine color based on selection
        SDL_Color color = (i == MS_selectedIndex) ? red : white;

        // Render the option
        MS_renderText(renderer, font, options->at(i), currentX, currentY, color);
    }
}


int handleMenuInputGrid(SDL_Event event, std::vector<std::string>* options) {
    if (event.type == SDL_KEYDOWN) {
        int numOptions = std::min(static_cast<int>(options->size()), 12); // Cap to 12 options
        const int maxColumns = 4;

        if (event.key.keysym.sym == SDLK_LEFT) {
            // Move left, wrapping around if necessary
            if (MS_selectedIndex % maxColumns > 0) {
                MS_selectedIndex--; // Move left
            }
            else {
                MS_selectedIndex = (MS_selectedIndex + maxColumns) % numOptions; // Wrap to the end of the row
            }
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            // Move right, wrapping around if necessary
            if (MS_selectedIndex % maxColumns < maxColumns - 1 && MS_selectedIndex < numOptions - 1) {
                MS_selectedIndex++; // Move right
            }
            else {
                MS_selectedIndex = (MS_selectedIndex + 1) % numOptions; // Wrap to the start of the next row
            }
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            // Move down, wrapping to the next row
            MS_selectedIndex = (MS_selectedIndex + maxColumns < numOptions) ? MS_selectedIndex + maxColumns : MS_selectedIndex;
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            // Move up, wrapping to the previous row
            MS_selectedIndex = (MS_selectedIndex - maxColumns >= 0) ? MS_selectedIndex - maxColumns : MS_selectedIndex;
        }
        else if (event.key.keysym.sym == SDLK_z) {
            // Perform action based on selected option
            gameState.selectionIndex = MS_selectedIndex + 1; // 0 is not selected anything yet!
            gameState.inMenu = false;
            /*
            if (gameState.callbackNPC) {
                gameState.callbackNPC->handleChoice(gameState.selectionIndex); // Notify the NPC about the selection.
            }
            */
        }
    }
    return MS_selectedIndex;
}







/*
* // Define the current menu state and selected index
MenuState currentMenu = MAIN_MENU;
int selectedIndex = 0;
int subMenuSelectionIndex = 0;
*/

void MS_renderMenu(SDL_Renderer* renderer, TTF_Font* font) {
    switch (currentMenu) {
    case MAIN_MENU:
        renderMainMenu(renderer, font);
        break;
    case INVENTORY_MENU:
        renderInventoryMenu(renderer, font);
        break;
    case ITEM_OPTIONS_MENU:
        renderItemOptionsMenu(renderer, font);
        break;
    case STATS_MENU:
        renderStatsMenu(renderer, font);
        break;
    }
}

void MS_handleMenuInput(SDL_Event event) {
    switch (currentMenu) {
    case MAIN_MENU:
        handleMainMenuSelection(event);
        break;
    case INVENTORY_MENU:
        handleInventoryMenuSelection(event);
        break;
    case ITEM_OPTIONS_MENU:
        handleItemOptionsMenuSelection(event);
        break;
    case STATS_MENU:
        handleStatsMenu(event);
        break;
    }
}

void renderMainMenu(SDL_Renderer* renderer, TTF_Font* font) {
    // Implement rendering for the main menu
    //std::vector<std::string> options = { "Check", "Items", "Stats" };
    renderMenuGrid(renderer, font, &MAIN_MENU_Options);
}

void renderInventoryMenu(SDL_Renderer* renderer, TTF_Font* font) {
    // Implement rendering for the inventory menu
    std::vector<std::string> options;
    for (int itemID : gameState.Inventory) {
        options.push_back(GetItemnameFromIndex(itemID));
    }

    renderMenuGrid(renderer, font, &options);
}

void renderItemOptionsMenu(SDL_Renderer* renderer, TTF_Font* font) {
    /*
    std::string itemName = GetItemnameFromIndex(gameState.Inventory.at(selectedIndex));
    std::vector<std::string> options = { itemName, " ", " ", " "}; // I want the item name in the first row alone.
    options.push_back("");
    renderMenuGrid(renderer, font, &options);
    */
    std::string itemName = GetItemnameFromIndex(gameState.Inventory.at(MS_selectedIndex));

    // Render the item name
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    MS_renderText(renderer, font, itemName, screenWidth / 2, screenHeight / 2 - 20, { 255, 255, 255 });

    // Define option names
    std::vector<std::string> options = { "Use", "View Info", "Throw Away" };

    // Calculate position for options
    int yOffset = screenHeight / 2 + 20;  // Adjust for space below item name

    // Render each option
    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Color color = (i == MS_selectedIndex) ? SDL_Color{ 237, 28, 36 } : SDL_Color{ 255, 255, 255 }; // Highlight color
        MS_renderText(renderer, font, options[i], screenWidth / 2, yOffset + i * 30, color); // Spacing options vertically
    }
}

void renderStatsMenu(SDL_Renderer* renderer, TTF_Font* font) {
    std::vector<std::string> options;
    char buffer[30];
    sprintf_s(buffer, "Kills: %d", gameState.kills);
    options.push_back(buffer);
    sprintf_s(buffer, "Money: %d", gameState.money);
    options.push_back(buffer);
    MS_renderTextBox(renderer);
    for (int i = 0; i < options.size(); i++) {
        SDL_Color color = { 255, 255, 255 };
        MS_renderText(renderer, font, options[i], 60, 410 + (i * 40), color);
    }
}

void handleMainMenuSelection(SDL_Event event) {
    // Handle input for the main menu
    std::vector<std::string> options = { "Check", "Items", "Stats" };
    switch (handleMenuInputGrid(event, &options)) {
    case 1:
        // check()
        break;

    case 2:
        currentMenu = INVENTORY_MENU;
        MS_selectedIndex = 0; // reset selection for next menu;
        break;

    case 3:
        currentMenu = STATS_MENU;
        MS_selectedIndex = 0; // reset
        break;

    default:
        // how did we get here?
        printf("\n[!] Error on handleMainMenuSelection() !!!\n");
    }
}

void handleInventoryMenuSelection(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_z) {
        currentMenu = ITEM_OPTIONS_MENU;  // Enter item options
    }
    else if (event.key.keysym.sym == SDLK_x) {
        currentMenu = MAIN_MENU;  // Back to main menu
    }
}

void handleItemOptionsMenuSelection(SDL_Event event) {
    std::vector<std::string> options = { "Use", "View Info", "Throw Away" };

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            MS_selectedIndex = (MS_selectedIndex > 0) ? MS_selectedIndex - 1 : options.size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            MS_selectedIndex = (MS_selectedIndex < options.size() - 1) ? MS_selectedIndex + 1 : 0;
        }
        else if (event.key.keysym.sym == SDLK_z) { // Confirm selection
            switch (MS_selectedIndex) {
            case 1: // Use
                std::cout << "Attempt to use item: " << GetItemnameFromIndex(MS_selectedIndex) << std::endl;
                break;
            case 2: // View Info
                std::cout << "Attempt to View info for: " << GetItemnameFromIndex(MS_selectedIndex) << std::endl;
                break;
            case 3:
                std::cout << "Attempt to Throw away item: " << GetItemnameFromIndex(MS_selectedIndex) << std::endl;
                break;
            default:
                printf("\n [!] ERROR: handleItemOptionsMenuSelection() selection index: %d", MS_selectedIndex);
            }
        }
        else if (event.key.keysym.sym == SDLK_x) { // Cancel action
            std::cout << "Returning to inventory." << std::endl;
            // Logic to return to the inventory menu
        }
    }
}

void handleStatsMenu(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_x) {
        currentMenu = MAIN_MENU;  // Enter item options
    }
}