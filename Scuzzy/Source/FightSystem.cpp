#include "Source/FightSystem.hpp"
#include "Source/Helper.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <random>
#include "Source/GameState.hpp"


// Modern random number generator
std::mt19937 rng;
bool rngInitialized = false;

// Selection index for menus
int selection = 0;

// Text to display in dialogue boxes
std::string fightText = "";

std::string GetItemnameFromIndex(int index);

// Initialize random number generator (call once at program start)
void initRandom() {
    if (!rngInitialized) {
        rng.seed(std::random_device()());
        rngInitialized = true;
    }
}

// Generate a random number between 1 and n
int chance(int n) {
    if (!rngInitialized) {
        initRandom();
    }
    std::uniform_int_distribution<int> dist(1, n);
    return dist(rng);
}

// Render the text box background
void FS_renderTextBox(SDL_Renderer* renderer) {
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int boxWidth = screenWidth * 0.9;  // 90% of the screen width
    int boxHeight = 300;               // Fixed height for the text box
    int xPos = (screenWidth - boxWidth) / 2;  // Center the box horizontally
    int yPos = screenHeight - boxHeight - 20; // Place the box 20 pixels above the bottom

    SDL_Rect textBoxTemp = { xPos, yPos, boxWidth, boxHeight };
    SDL_Rect blacktemp = textBoxTemp; // this is inside the red box to be a background
    blacktemp.x = blacktemp.x + 5;
    blacktemp.y = blacktemp.y + 5;
    blacktemp.w = blacktemp.w - 10;
    blacktemp.h = blacktemp.h - 10;

    SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
    SDL_RenderFillRect(renderer, &textBoxTemp);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &blacktemp);
}

// Render text at a specific position
void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Render text inside the text box
void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color) {
    FS_renderTextBox(renderer);
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box
    SDL_Rect dstRect = { xOffset, yOffset, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// State handler functions
void HandleIntroState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // Display initial enemy dialogue
    FS_renderText(renderer, font, gameState.enemy->m_EnemyDialogue[gameState.Plot], { 255, 255, 255 });

    // Press Z to advance
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        gameState.Plot++;
        gameState.fightState = FightState::PLAYER_TURN_MENU;
        selection = 0; // Reset selection for menu


        //// If we've reached the end of intro dialogue, move to player's turn
        //if (gameState.Plot >= gameState.enemy->m_EnemyDialogue.size()) {
        //    gameState.fightState = FightState::PLAYER_TURN_MENU;
        //    selection = 0; // Reset selection for menu
        //}
    }
}

void HandlePlayerTurnMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderTextBox(renderer);

    // Define menu options
    std::vector<std::string> fightMenu = { "Fight", "Actions", "Items" };

    // Render menu options
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;
    int yOffset = screenHeight - 275;

    for (int i = 0; i < fightMenu.size(); i++) {
        SDL_Color color = (i == selection) ? SDL_Color{ 237, 28, 36 } : SDL_Color{ 255, 255, 255 };
        FS_renderText(renderer, font, fightMenu[i], xOffset + (i * 300), yOffset, color);
    }

    // Handle input
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            selection--;
            if (selection < 0) selection = fightMenu.size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            selection++;
            if (selection >= fightMenu.size()) selection = 0;
        }
        else if (event.key.keysym.sym == SDLK_z) {
            // Handle selection
            switch (selection) {
            case 0: // Fight
                // Attack enemy
                gameState.enemy->HP -= 1; // Replace with proper damage calculation

                // Set text for result dialogue
                fightText = "You hit the " + std::string("{ENEMY ID: ") +
                    std::to_string(gameState.enemy->m_EnemyID) + " }\n";

                // Check if enemy defeated
                if (gameState.enemy->HP <= 0) {
                    gameState.enemy->alive = false;
                    fightText = "You defeated the enemy!";
                    gameState.kills++;
                    gameState.money += chance(10);
                    gameState.fightState = FightState::FIGHT_END;
                }
                else {
                    gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                }
                break;

            case 1: // Actions
                gameState.fightState = FightState::PLAYER_ACTIONS_MENU;
                selection = 0; // Reset selection for new menu
                break;

            case 2: // Items
                gameState.fightState = FightState::PLAYER_ITEMS_MENU;
                selection = 0; // Reset selection for new menu
                break;
            }
            gameState.turnCount++;
        }
    }
}

void HandlePlayerActionsMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderTextBox(renderer);

    // Get actions for current enemy
    std::vector<std::string> actionMenu = gameState.enemy->m_Actions;

    // Render menu options
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;
    int yOffset = screenHeight - 275;

    for (int i = 0; i < actionMenu.size(); i++) {
        SDL_Color color = (i == selection) ? SDL_Color{ 237, 28, 36 } : SDL_Color{ 255, 255, 255 };
        FS_renderText(renderer, font, actionMenu[i], xOffset + (i * 300), yOffset, color);
    }

    // Handle input
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            selection--;
            if (selection < 0) selection = actionMenu.size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            selection++;
            if (selection >= actionMenu.size()) selection = 0;
        }
        else if (event.key.keysym.sym == SDLK_z) {
            // Set text based on selected action
            if (selection < gameState.enemy->m_ActionResponse.size()) {
                fightText = gameState.enemy->m_ActionResponse[selection];
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                gameState.turnCount++;
                selection = 0; // Set to Actions option
            }
            else {
                fightText = "ERROR: Invalid action!";
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
            }
        }
        else if (event.key.keysym.sym == SDLK_x) {
            // Go back to main menu
            gameState.fightState = FightState::PLAYER_TURN_MENU;
            selection = 1; // Set to Actions option
        }
    }
}

void HandlePlayerItemsMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderTextBox(renderer);

    // TODO: Implement inventory display
    std::vector<std::string> itemMenu = { "No items available" };
	if (gameState.Inventory.size() > 0) {
		itemMenu.clear();
        for (int i = 0; i < gameState.Inventory.size(); i++) {
            itemMenu.push_back(GetItemnameFromIndex(gameState.Inventory[i]));
        }
	}
	int numOptions = itemMenu.size();

    // Render menu options
    int screenWidth, screenHeight;
    
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int boxWidth = screenWidth * 0.9;
    int xOffset = screenWidth * 0.05 + 30;
    int yOffset = screenHeight - 275;

    SDL_Color white = { 255, 255, 255 };  // Normal text color
    SDL_Color red = { 237, 28, 36 };       // Highlighted text color


    // Calculate grid dimensions
    const int maxColumns = 4;
    const int maxRows = (itemMenu.size() + maxColumns - 1) / maxColumns; // Calculate necessary rows
    int optionWidth = (boxWidth - (maxColumns + 1) * 10) / maxColumns; // 10 is spacing
    int optionHeight = 50; // Set a fixed height for each option
    for (int i = 0; i < itemMenu.size(); i++) {
        int column = i % 4;  // Current column (0, 1, 2, 3)
        int row = i / 4;      // Current row

        int currentX = xOffset + (column * (optionWidth + 10)); // 10 is the spacing
        int currentY = yOffset + (row * (optionHeight + 10));   // 10 is the spacing

        // Determine color based on selection
        SDL_Color color = (i == selection) ? red : white;

        // Render the option
        FS_renderText(renderer, font, itemMenu[i], currentX, currentY, color);
    }


    // Handle input
    if (event.type == SDL_KEYDOWN) { 
        if (event.key.keysym.sym == SDLK_x) {
            // Go back to main menu
            gameState.fightState = FightState::PLAYER_TURN_MENU;
            selection = 2; // Set to Items option
        }
        else if (event.key.keysym.sym == SDLK_z && gameState.Inventory.size() > 0) { // TODO: Add item selection and usage logic
			gameState.fightState = FightState::RESULT_DIALOGUE;

            //gameState.fightState = FightState::PLAYER_TURN_MENU;
			fightText = "You used the ";
			fightText += GetItemnameFromIndex(gameState.Inventory[selection]);
			UseItem(gameState.Inventory[selection]); // Use the selected item
            
            //selection = 2; // Set to Items option

			// Remove item from inventory
            //if (gameState.Inventory.size() > 0) {
                gameState.Inventory.erase(gameState.Inventory.begin() + selection);
            //}

            selection = 2; // Set to Items option
        }

  //      // mutiple items would probably break this. EDIT: thought so.
		//else if (event.key.keysym.sym == SDLK_UP) {
		//	selection--;
		//	if (selection < 0) selection = itemMenu.size() - 1;
		//}
		//else if (event.key.keysym.sym == SDLK_DOWN) {
		//	selection++;
		//	if (selection >= itemMenu.size()) selection = 0;
		//}


        if (event.key.keysym.sym == SDLK_LEFT) {
            // Move left, wrapping around if necessary
            if (selection % maxColumns > 0) {
                selection--; // Move left
            }
            else {
                selection = (selection + maxColumns) % numOptions; // Wrap to the end of the row
            }
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            // Move right, wrapping around if necessary
            if (selection % maxColumns < maxColumns - 1 && selection < numOptions - 1) {
                selection++; // Move right
            }
            else {
                selection = (selection + 1) % numOptions; // Wrap to the start of the next row
            }
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            // Move down, wrapping to the next row
            selection = (selection + maxColumns < numOptions) ? selection + maxColumns : selection;
        }
        else if (event.key.keysym.sym == SDLK_UP) {
            // Move up, wrapping to the previous row
            selection = (selection - maxColumns >= 0) ? selection - maxColumns : selection;
        }

        
    }
}

void HandlePlayerActionResultState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderText(renderer, font, fightText, { 255, 255, 255 });

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        // Move to enemy's turn
        gameState.fightState = FightState::ENEMY_TURN;
    }
}

void HandleFightEndState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event); // coupon clipping bastard
void HandleEnemyTurnState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // Determine enemy action
    int action = chance(8);

    if (gameState.enemyID > 0 && gameState.enemyID < 5) {
        // Boss-fight specific logic would go here
        // For now, just move to dialogue
        gameState.fightState = FightState::ENEMY_DIALOGUE;
        return;
    }

    // Regular enemy logic
    if (action == 1) {
        // Player dodges

		gameState.fightState = FightState::DODGE_MECHANIC;
        //fightText = "You dodged the attack!";
        //gameState.fightState = FightState::RESULT_DIALOGUE;
    }
    else if (action >= 4 && action <= 6) {
        // Enemy speaks dialogue
        if (gameState.enemy->m_EnemyDialogue.size() > 0) {
            int dialogueIndex = chance(gameState.enemy->m_EnemyDialogue.size() - 1);
            fightText = gameState.enemy->m_EnemyDialogue[dialogueIndex];
        }
        else {
            //fightText = "The enemy glares at you silently.";
			fightText = gameState.enemy->m_EnemyDialogue[1 + rand() % (gameState.enemy->m_EnemyDialogue.size() - 1 )];
        }
        gameState.fightState = FightState::ENEMY_DIALOGUE;
    }
    else {
        // Player takes damage
        //gameState.fightState = FightState::DODGE_MECHANIC;
        //gameState.HP -= gameState.enemy->m_AttackDamage;
        fightText = "You took " + std::to_string(gameState.enemy->m_AttackDamage) + " damage!\n";

        if (gameState.HP <= 0) {
            gameState.HP = 0;
            fightText += "You were defeated!";
            FS_renderText(renderer, font, fightText, { 255, 255, 255 });
            gameState.fightState = FightState::FIGHT_END;
			gameState.dead = true;
			HandleFightEndState(renderer, font, event);
        }
        else {
            gameState.fightState = FightState::DODGE_MECHANIC;
            //gameState.fightState = FightState::RESULT_DIALOGUE;
        }
    }
}

void HandleEnemyDialogueState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderText(renderer, font, fightText, { 255, 255, 255 });

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        gameState.fightState = FightState::RESULT_DIALOGUE;
    }
}

void HandleResultDialogueState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderText(renderer, font, fightText, { 255, 255, 255 });

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        // Return to player's turn
        gameState.fightState = FightState::PLAYER_TURN_MENU;
    }
}

void HandleFightEndState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    gameState.inFight = false;
    gameState.wonFight = !gameState.dead;
	gameState.Text.clear();
	if (!gameState.dead) {
        gameState.Text = { "You won! FUCKJ " };
        gameState.textAvailable = true;
	}
	else {
        //gameState.Text = { "You lost the fight!" };
	}
	//gameState.textAvailable = true;

  //  FS_renderText(renderer, font, fightText, { 255, 255, 255 });

  //  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
  //      // Exit fight mode
  //      gameState.inFight = false;
		//gameState.wonFight = !gameState.dead;
  //  }
}

void HandleDodgeingMechanic(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // render fight
    // get attack from enemy
    // attack
    // end turn on timer..?
	printf("DODGE MECHANIC HERE\n");
    // how do i impliment attacks? i need projectiles and rules for them.
    // projectile class..? custom update function per projectile..?
    // also I like deltarune's TP mechanic, I think near misses are really cool and unexplored
	// tension meter but for attacking or effectiveness of attacks?
	// tension meter that builds up as you dodge attacks, when full your next attack does more damage?
	// tension meter that helps you get access to actions?
	// silly mode needs to be acheived in actions, and locking them behind tension meter is kinda cool. 
    // ROLLING HP METER line earthbound too. 
    if (!gameState.fightTurnTimer.isStarted()) {
        gameState.fightTurnTimer.start();
		gameState.lastTurnTime = SDL_GetTicks();
		printf("DODGE TIME STARTED\n");
    }
    else {
        if (SDL_GetTicks() - gameState.lastTurnTime >= gameState.turnTimeLimit) {
            // time's up, end turn
            gameState.fightTurnTimer.stop();
            //gameState.fightState = FightState::ENEMY_TURN;

			gameState.fightState = FightState::RESULT_DIALOGUE;
			printf("DODGE TIME UP, ENEMY TURN\n");
		}
        else {
			// still the enemy's turn, update projectiles and handle player position.
			printf("DODGE TIME ONGOING!\n");
        }
    }
}



// Main fight system input handler
void FS_HandleInput(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // Always display HP regardless of state
    std::string HP = "HP: " + std::to_string(gameState.HP);
    FS_renderText(renderer, font, HP, 200, 700, { 237, 28, 36 });

    std::string enemyHP = "Enemy HP: " + std::to_string(gameState.enemy->HP);
    FS_renderText(renderer, font, enemyHP, 200, 50, { 237, 28, 36 });

    // Handle input and rendering based on current state
    switch (gameState.fightState) {
    case FightState::INTRO:
        HandleIntroState(renderer, font, event);
        break;

    case FightState::PLAYER_TURN_MENU:
        HandlePlayerTurnMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ACTIONS_MENU:
        HandlePlayerActionsMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ITEMS_MENU:
        HandlePlayerItemsMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ACTION_RESULT:
        HandlePlayerActionResultState(renderer, font, event);
        break;

    case FightState::ENEMY_TURN:
        HandleEnemyTurnState(renderer, font, event);
        break;
	case FightState::DODGE_MECHANIC:
        HandleDodgeingMechanic(renderer, font, event);
		break;

    case FightState::ENEMY_DIALOGUE:
        HandleEnemyDialogueState(renderer, font, event);
        break;

    case FightState::RESULT_DIALOGUE:
        HandleResultDialogueState(renderer, font, event);
        break;

    case FightState::FIGHT_END:
        HandleFightEndState(renderer, font, event);
        break;
    }
}

// Initialize fight system
void FS_InitFight() {
    // Reset fight state variables
    selection = 0;
    fightText = "";
    gameState.Plot = 0;
    gameState.turnCount = 0;
    gameState.inFight = true;
    gameState.wonFight = false;
    gameState.inMenu = false;

    // Set initial state
    gameState.fightState = FightState::INTRO;

    // Initialize RNG if not already done
    if (!rngInitialized) {
        initRandom();
    }
}