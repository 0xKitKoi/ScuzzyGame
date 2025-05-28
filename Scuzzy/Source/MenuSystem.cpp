

#include "Source/MenuSystem.hpp"
#include "Source/GameState.hpp"
#include "Source/Enums.hpp"
#include "Source/Helper.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

#ifdef _WIN32
#define sprintf_s sprintf_s
#else
#define sprintf_s snprintf
#endif


//extern const int SCREEN_WIDTH;
//extern const int SCREEN_HEIGHT;

// Define the current menu state and selected index
MenuState currentMenu = MAIN_MENU;
MenuState lastMenuState = MAIN_MENU; // To keep track of the last menu state
int MS_selectedIndex = 0;
int subMenuSelectionIndex = 0;
std::vector<std::string> MAIN_MENU_Options = { "Check", "Items", "Stats" };


//std::string GetItemnameFromIndex(int index) {
//    switch (index) {
//    case 0:
//        return "Test Item 1";
//        break;
//    case 1:
//        return "Test Item 2";
//        break;
//    default:
//        printf("\n [!] ERROR: Could not get Item name at selection index: %d", index);
//        return "ERROR";
//        break;
//    }
//}


void MS_renderTextBox(SDL_Renderer* renderer) {

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    int boxWidth = screenWidth * 0.9;  // 90% of the screen width
    int boxHeight = 300;               // Fixed height for the text box
    int xPos = (screenWidth - boxWidth) / 2;  // Center the box horizontally
    int yPos = screenHeight - boxHeight - 20; // Place the box 20 pixels above the bottom

    //// Set the color for the text box (e.g., black)
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_Rect textBoxRect = { xPos, yPos, boxWidth, boxHeight };
    //SDL_RenderFillRect(renderer, &textBoxRect);

    //// Draw a red border around the text box
    //SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &textBoxRect);

    SDL_Rect textBoxTemp = { xPos, yPos, boxWidth, boxHeight };
    SDL_Rect blacktemp = textBoxTemp;
    blacktemp.x = blacktemp.x + 5;
    blacktemp.y = blacktemp.y + 5;
    blacktemp.w = blacktemp.w - 10;
    blacktemp.h = blacktemp.h - 10;


    SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    SDL_RenderFillRect(renderer, &textBoxTemp);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &blacktemp);
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
    int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
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

// i got this grid, and i need to get the item id from the selection. the selection index is different from the item ID.
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
        
        //else if (event.key.keysym.sym == SDLK_z) {
        //    // Perform action based on selected option
        //    gameState.selectionIndex = MS_selectedIndex + 1; // 0 is not selected anything yet!
        //    //gameState.inMenu = false;
        //    /*
        //    if (gameState.callbackNPC) {
        //        gameState.callbackNPC->handleChoice(gameState.selectionIndex); // Notify the NPC about the selection.
        //    }
        //    */
        //    return MS_selectedIndex + 1;
        //}

    }
    else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_z) {
        gameState.selectionIndex = MS_selectedIndex;
        return MS_selectedIndex + 1;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x && currentMenu != MAIN_MENU) {
        //gameState.inMenu = false;
		// skip over this, erroneous
        return 0; // who fucking wrote this shit LMFAOOOOOO
    }
    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x && currentMenu == MAIN_MENU) {
        gameState.inMenu = false;
		//MS_selectedIndex = 0;
		return 0;
    }
    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x) {
        lastMenuState = currentMenu; // Save the last menu state
        currentMenu = MAIN_MENU;
		
        MS_selectedIndex = 0;
        return 0;
    }
    return 0;
}


void renderResponse(SDL_Renderer* renderer, TTF_Font* font) {
    // Get screen dimensions
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    // Render the text box at the bottom of the screen
    MS_renderTextBox(renderer);

    // Calculate text rendering position
    int boxWidth = screenWidth * 0.9;
    int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box
	MS_renderText(renderer, font, gameState.Text[gameState.textIndex], xOffset, yOffset, {255, 255, 255});
}

void handleResponse(SDL_Event event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z || event.key.keysym.sym == SDLK_x) {
        /*if (gameState.shouldAnimateText && gameState.textAnimating) {
            // If text is still animating, show the full text immediately
            gameState.currentDisplayText = gameState.Text[gameState.textIndex];
            gameState.textAnimating = false;
        }
        else
        if (gameState.textIndex < gameState.Text.size() - 1) {
            // Move to next line and start animating if needed
            gameState.textIndex++;
            if (gameState.shouldAnimateText) {
                gameState.currentCharIndex = 0;
                gameState.textTimer = 0.0f;
                gameState.textAnimating = true;
                gameState.currentDisplayText = "";
            }
        }
        else */ 
            // Response finished
            gameState.textIndex = 0;
            gameState.Text.clear();
            gameState.textAvailable = false;
            gameState.textAnimating = false;
            currentMenu = lastMenuState; // go back
        
    }
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
	case RESPONSE:
		renderResponse(renderer, font);
        break;
    }
}

void MS_handleMenuInput(SDL_Event event) {
    /*
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
    */
    if (currentMenu == MAIN_MENU) {
        handleMainMenuSelection(event);
    }
    else if (currentMenu == INVENTORY_MENU) {
        handleInventoryMenuSelection(event);
    }
    else if (currentMenu == ITEM_OPTIONS_MENU) {
        handleItemOptionsMenuSelection(event);
    }
    else if (currentMenu == STATS_MENU) {
        handleStatsMenu(event);
    }
	else if (currentMenu == RESPONSE) {
		handleResponse(event);
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
    int screenWidth, screenHeight;
    //SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
	SDL_GetWindowSize(SDL_GetWindowFromID(1), &screenWidth, &screenHeight); // Get the window size


    int xOffset = screenWidth * 0.05 + 10;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box

    int boxWidth = screenWidth * 0.9;  // 90% of the screen width
    int boxHeight = 300;               // Fixed height for the text box
    //int xPos = (screenWidth - boxWidth) / 2;  // Center the box horizontally
    //int yPos = (screenHeight - boxHeight) / 2; // Place the box 20 pixels above the bottom
    //SDL_Rect textBoxTemp = { xPos, yPos, boxWidth, boxHeight };
    //SDL_Rect blacktemp = textBoxTemp;
    //blacktemp.x = blacktemp.x + 10;
    //blacktemp.y = blacktemp.y + 10;
    //blacktemp.w = blacktemp.w - 20;
    //blacktemp.h = blacktemp.h - 20;


    //SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    //SDL_RenderFillRect(renderer, &textBoxTemp);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderFillRect(renderer, &blacktemp);


    if (gameState.Inventory.empty()) {
        MS_renderTextBox(renderer);
        // TODO: puyt the damn text in a textbox plsss
        MS_renderText(renderer, font, "* You don't have any items.", xOffset, yOffset, { 255, 255, 255 });
        //currentMenu = MAIN_MENU;
        return;
    }
    else {

        //for (int itemID : gameState.Inventory) {
        //    options.push_back(GetItemnameFromIndex(itemID));
        //}
        for (int i = 0; i < gameState.Inventory.size(); i++) {
            options.push_back(GetItemnameFromIndex(i));
        }
    }
        // not used..?

        int numOptions = std::min(static_cast<int>(gameState.Inventory.size()), 12); // Cap to 12 options

        // Calculate grid dimensions
        //const int maxColumns = 4;
        //const int maxRows = (numOptions + maxColumns - 1) / maxColumns; // Calculate necessary rows
        //int optionWidth = (boxWidth - (maxColumns + 1) * 10) / maxColumns; // 10 is spacing
        //int optionHeight = 50; // Set a fixed height for each option

        //SDL_Color white = { 255, 255, 255 };  // Normal text color
        //SDL_Color red = { 237, 28, 36 };       // Highlighted text color

        //for (int i = 0; i < numOptions; i++) {
        //    int column = i % maxColumns;  // Current column (0, 1, 2, 3)
        //    int row = i / maxColumns;      // Current row

        //    int currentX = xOffset + (column * (optionWidth + 10)); // 10 is the spacing
        //    int currentY = yOffset + (row * (optionHeight + 10));   // 10 is the spacing

        //    // Determine color based on selection
        //    SDL_Color color = (i == MS_selectedIndex) ? red : white;

        //    // Render the option
        //    MS_renderText(renderer, font, options.at(i), currentX, currentY, color);
        //}

    renderMenuGrid(renderer, font, &options);
}

void renderItemOptionsMenu(SDL_Renderer* renderer, TTF_Font* font) {
    /*
    std::string itemName = GetItemnameFromIndex(gameState.Inventory.at(selectedIndex));
    std::vector<std::string> options = { itemName, " ", " ", " "}; // I want the item name in the first row alone.
    options.push_back("");
    renderMenuGrid(renderer, font, &options);
    */

    // stinky ah 

    if (gameState.Inventory.empty()) {
        return;
    }
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int boxWidth = screenWidth * 0.9;  // 90% of the screen width
    int boxHeight = 300;               // Fixed height for the text box
    int xPos = (screenWidth - boxWidth) / 2;  // Center the box horizontally
    int yPos = (screenHeight - boxHeight) / 2; // Place the box 20 pixels above the bottom
    SDL_Rect textBoxTemp = { xPos, yPos, boxWidth, boxHeight };
    SDL_Rect blacktemp = textBoxTemp;
    blacktemp.x = blacktemp.x + 10;
    blacktemp.y = blacktemp.y + 10;
    blacktemp.w = blacktemp.w - 20;
    blacktemp.h = blacktemp.h - 20;


    SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    SDL_RenderFillRect(renderer, &textBoxTemp);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &blacktemp);


    /*
    if (MS_selectedIndex >= gameState.Inventory.size()) {
        MS_selectedIndex = gameState.Inventory.size()-1;
    }*/
    //std::string itemName = GetItemnameFromIndex(gameState.Inventory.at(MS_selectedIndex) );
    //std::string itemName = GetIt  emnameFromIndex(gameState.Inventory.at(gameState.selectionIndex));
    std::string itemName;
    if (subMenuSelectionIndex >= 0 && subMenuSelectionIndex < gameState.Inventory.size()) {
        itemName = GetItemnameFromIndex(gameState.Inventory.at(subMenuSelectionIndex)); // LMFAOOOOOOOOOOOOOO loser 
        // Ive spent weeks over this shitty indexing issue. It's always indexing.
        // turns out I got smart and decided to hardcode the item ID in my save file to be+1 
        // and forgot about it. I did this to compensate for MS_SelectionIndex in the grid input handling func
        // this has pained me for three weeks. im in so much agony
    }
    else {
        itemName = std::to_string(gameState.Inventory.at(subMenuSelectionIndex));
    }

    // Render the item name
    //int screenWidth, screenHeight;
    //SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
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
    //sprintf_s(buffer, "Kills: %d", gameState.kills);
    //
    sprintf_s(buffer, sizeof(buffer), "Kills: %d", gameState.kills);
    options.push_back(buffer);
    sprintf_s(buffer, sizeof(buffer), "Money: %d", gameState.money);
    options.push_back(buffer);
    options.push_back("\nText can be here too!");

    MS_renderTextBox(renderer);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box

    //for (int i = 0; i < numOptions; i++) {
//    int column = i % maxColumns;  // Current column (0, 1, 2, 3)
//    int row = i / maxColumns;      // Current row

//    int currentX = xOffset + (column * (optionWidth + 10)); // 10 is the spacing
//    int currentY = yOffset + (row * (optionHeight + 10));   // 10 is the spacing

//    // Determine color based on selection
//    SDL_Color color = (i == MS_selectedIndex) ? red : white;

//    // Render the option
//    MS_renderText(renderer, font, options.at(i), currentX, currentY, color);
//}

    for (int i = 0; i < options.size(); i++) {
        SDL_Color color = { 255, 255, 255 };
        
        int row = i % options.size();
        int currentY = yOffset + (row * (30)); //?


        MS_renderText(renderer, font, options[i], xOffset, currentY, color);
    }
}

void handleMainMenuSelection(SDL_Event event) {
    // Handle input for the main menu
    std::vector<std::string> options = { "Check", "Items", "Stats" };
	if (event.key.repeat != 0) {
		return; // Ignore repeated key events
	}
    if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_x && currentMenu != MAIN_MENU) {
        gameState.inMenu = false;
        return;
    }


    switch (handleMenuInputGrid(event, &options)) {
    case 0:
        // no selection?
        //printf("\nnothing\n");
        break;

    case 1:
        // check()
        printf("Attempted to check()! \n");
        gameState.checkFlag = true;
        gameState.inMenu = false;
		lastMenuState = currentMenu;
        currentMenu = MAIN_MENU;
        break;

    case 2:
        lastMenuState = currentMenu;
        currentMenu = INVENTORY_MENU;
        MS_selectedIndex = 0; // reset selection for next menu;
        break;

    case 3:
        lastMenuState = currentMenu;
        currentMenu = STATS_MENU;
        MS_selectedIndex = 0; // reset
        break;

    default:
        // how did we get here?
        printf("\n[!] Error on handleMainMenuSelection() !!!\n");
    }
}

bool zPressed = false;
void handleInventoryMenuSelection(SDL_Event event) {
    //MS_selectedIndex = 0;
    // do item selections!
    std::vector<std::string> options;
    if (gameState.Inventory.empty()) {
        // no items
        //printf("No items what are we doign here");
		if (event.key.keysym.sym == SDLK_x) {
            lastMenuState = currentMenu;
			currentMenu = MAIN_MENU;
			MS_selectedIndex = 0;
		}
        return;
    }
    for (int i = 0; i < gameState.Inventory.size(); i++) {
        options.push_back(GetItemnameFromIndex(i));
    }
    //for (int id : gameState.Inventory) { // WHY IS id FUCKING ONE ON FIRST ITERATION FUCKYOUFUCKYOUFUCKYOUFUCKYOU
    //    options.push_back(GetItemnameFromIndex(id));
    //}

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            MS_selectedIndex = (MS_selectedIndex > 0) ? MS_selectedIndex - 1 : options.size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            MS_selectedIndex = (MS_selectedIndex < options.size() - 1) ? MS_selectedIndex + 1 : 0;
        }

        if (event.key.keysym.sym == SDLK_z) {
            lastMenuState = currentMenu;
            currentMenu = ITEM_OPTIONS_MENU;  // Enter item options
            gameState.selectionIndex = MS_selectedIndex;
            subMenuSelectionIndex = MS_selectedIndex;
            printf("Selected item index :   %d", MS_selectedIndex);
            MS_selectedIndex = 0;
        }
        else if (event.key.keysym.sym == SDLK_x) {
            lastMenuState = currentMenu;
            currentMenu = MAIN_MENU;  // Back to main menu
            MS_selectedIndex = 0;
        }




        /*
        else if (event.key.keysym.sym == SDLK_z && !zPressed) { // Confirm selection
            zPressed = true;
            currentMenu == ITEM_OPTIONS_MENU;
            MS_selectedIndex++;

            // printf("\n [!] ERROR: handleInventoryMenuSelection() selection index: %d", MS_selectedIndex);
        }*/
        //else if (event.key.keysym.sym == SDLK_x) { // Cancel action
        //    //zPressed = false;
        //    std::cout << "Returning to inventory." << std::endl;
        //    currentMenu = INVENTORY_MENU;
        //    // Logic to return to the inventory menu
        //}
    }
    //else if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_z) {
    //    //zPressed = false;  // Reset zPressed when the key is released
    //    currentMenu == ITEM_OPTIONS_MENU;
    //    //MS_selectedIndex++;
    //    gameState.selectionIndex = MS_selectedIndex;
    //    subMenuSelectionIndex = MS_selectedIndex;
    //    printf("BEUHHHHHHHHHHHHHHHHHHHHHHHHH :   %d", MS_selectedIndex);
    //    //MS_selectedIndex = 0;
    //    
    //}
    //else if (event.type == SDL_KEYUP &&  event.key.keysym.sym == SDLK_x) { // Cancel action
    //    //zPressed = false;
    //    //std::cout << "Returning to inventory." << std::endl;
    //    //currentMenu = INVENTORY_MENU;
    //    currentMenu = MAIN_MENU;
    //    MS_selectedIndex = 0; 
    //    // Logic to return to the inventory menu
    //}

     
    //if (event.key.keysym.sym == SDLK_z) {
    //    currentMenu = ITEM_OPTIONS_MENU;  // Enter item options
    //}
    //else if (event.key.keysym.sym == SDLK_x) {
    //    currentMenu = MAIN_MENU;  // Back to main menu
    //}
}



// TODO: fix vector index overflow error with MS_selectedIndex!!!!!!!!!
void handleItemOptionsMenuSelection(SDL_Event event) {
    std::vector<std::string> options = { "Use", "View Info", "Throw Away" };

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_UP) {
            MS_selectedIndex = (MS_selectedIndex > 0) ? MS_selectedIndex - 1 : options.size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            MS_selectedIndex = (MS_selectedIndex < options.size() - 1) ? MS_selectedIndex + 1 : 0;
        }
        else if (event.key.keysym.sym == SDLK_z) { // Confirm selection
            switch (MS_selectedIndex + 1) {
            case 1: // Use
                std::cout << "Attempt to use item: " << GetItemnameFromIndex(gameState.selectionIndex) << std::endl;
				gameState.Text = { "You used " + GetItemnameFromIndex(gameState.selectionIndex) + "." }; // Set response text
				gameState.textIndex = 0; // Reset text index

				UseItem(gameState.Inventory[gameState.selectionIndex]); // Use the item
				gameState.Inventory.erase(gameState.Inventory.begin() + gameState.selectionIndex); // Remove the item from inventory
                lastMenuState = INVENTORY_MENU;
                currentMenu = RESPONSE;
                break;
            case 2: // View Info
                std::cout << "Attempt to View info for: " << GetItemnameFromIndex(gameState.selectionIndex) << std::endl;
                break;
            case 3:
                std::cout << "Attempt to Throw away item: " << GetItemnameFromIndex(gameState.selectionIndex) << std::endl;
                break;
            default:
                printf("\n [!] ERROR: handleItemOptionsMenuSelection() selection index: %d", gameState.selectionIndex);
            }
        }
        else if (event.key.keysym.sym == SDLK_x) { // Cancel action
            std::cout << "Returning to inventory." << std::endl;
            lastMenuState = currentMenu;
            currentMenu = INVENTORY_MENU;
            // Logic to return to the inventory menu
        }
    }
}

void handleStatsMenu(SDL_Event event) {
    if (event.key.keysym.sym == SDLK_x) {
        lastMenuState = currentMenu;
        currentMenu = MAIN_MENU;  // Enter item options
    }
}
