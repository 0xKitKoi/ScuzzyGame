#include "Source/FightSystem.hpp"
#include "Source/Helper.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <random>
#include "Source/GameState.hpp"
#include "Source/Item.hpp"
#include "Source/BackgroundLayer.h"
#include "Source/Magic.hpp"


//extern std::shared_ptr<BackgroundLayer> bgLayer1;
//extern std::shared_ptr<BackgroundLayer> bgLayer2;

// Modern random number generator
std::mt19937 rng;
bool rngInitialized = false;

// Selection index for menus
int selection = 0;

// Text to display in dialogue boxes
std::string fightText = "";

// Fight-specific animated text state (do NOT use gameState.Text for fight text)
std::string fightFullText = "";           // full string for the current queued fight line
std::string fightCurrentDisplay = "";      // currently visible substring (typewriter)
int fightCurrentCharIndex = 0;               // index into full string
float fightTextTimer = 0.0f;                 // timer accumulator for animation
bool fightTextAnimating = false;             // whether fight text is currently animating
bool fightShouldAnimateText = false;         // whether to animate the next queued text

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
    if (n <= 0) {
        // Avoid invalid distribution bounds. Returning 0 means 'none' for callers that expect 1..n.
        printf("Warning: chance() called with n=%d\n", n);
        return 0;
    }
    if (!rngInitialized) {
        initRandom();
    }
    std::uniform_int_distribution<int> dist(1, n);
    return dist(rng);
}

int chance(int i, int j) {
    // random number in range
    if (i <= 0 || j <= 0) {
        printf("Warning! : range chance() called with i:%d, j:%d !!!!\n", i, j);
        return 0;
    }
    if (!rngInitialized) { initRandom(); }
    std::uniform_int_distribution<int> dist(i, j);
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
    if (!renderer || !font) return; // Defensive: ensure we have what we need
    if (text.empty()) return; // Nothing to render

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        printf("TTF_RenderText_Solid failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Render text inside the text box
void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color) {
    FS_renderTextBox(renderer);
    if (!renderer || !font) return; // Defensive: ensure we have what we need
    if (text.empty()) return; // Nothing to render inside box

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        printf("TTF_RenderText_Solid failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
    int yOffset = screenHeight - 275;       // Place the text inside the box
    SDL_Rect dstRect = { xOffset, yOffset, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Helper: clear any queued/dialogue animation (use when entering input-driven states)
void FS_ClearFightTextQueue() {
    // Clear only fight-local animation state so we don't interfere with global/dialogue text
    fightFullText.clear();
    fightCurrentDisplay.clear();
    fightCurrentCharIndex = 0;
    fightTextTimer = 0.0f;
    fightTextAnimating = false;
    fightShouldAnimateText = false;
    fightText.clear();
}

// Helper: queue fight text for animated display
void FS_QueueFightText(const std::string& text) {
    // Use fight-local buffers so we don't affect global NPC/dialogue flow
    fightFullText = text;
    fightText = text; // keep top-level fightText as a fallback/full-text holder
    fightCurrentCharIndex = (text.empty() ? 0 : 1);
    fightTextTimer = 0.0f;
    fightTextAnimating = true;
    fightShouldAnimateText = true;
    fightCurrentDisplay = (text.empty() ? std::string() : text.substr(0, fightCurrentCharIndex));
}

// Update animation state and render the current (possibly partial) display text.
// event is passed so skipping (X) can be handled immediately.
void FS_UpdateAndRenderAnimatedText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, SDL_Event event) {
    // Draw text box and render inside it
    FS_renderTextBox(renderer);

    // If we have fight-local animation data, process it
    if (fightShouldAnimateText && !fightFullText.empty()) {
        const std::string& full = fightFullText;

        // Skip animation if X pressed
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x) {
            if (fightTextAnimating) {
                fightCurrentDisplay = full;
                fightTextAnimating = false;
            }
        }

        // Advance animation using frame delta
        if (fightTextAnimating) {
            fightTextTimer += gameState.deltaTime; // deltaTime is seconds per frame
            while (fightTextTimer >= gameState.textSpeed && fightCurrentCharIndex < (int)full.size()) {
                fightCurrentCharIndex++;
                fightCurrentDisplay = full.substr(0, fightCurrentCharIndex);
                fightTextTimer -= gameState.textSpeed;
            }
            if (fightCurrentCharIndex >= (int)full.size()) {
                fightTextAnimating = false;
            }
        }

        // Render current display text inside the text box
        FS_renderText(renderer, font, fightCurrentDisplay, color);
    }
    else {
        // No animation requested; fallback to showing fightText directly (also rendered inside box)
        FS_renderText(renderer, font, fightText, color);
    }
}

// State handler functions
void HandleIntroState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // Ensure the current intro dialogue is queued for animated display
    if (gameState.enemy && gameState.Plot < gameState.enemy->m_EnemyDialogue.size()) {
        // Use fight-local buffer to decide whether to (re-)queue text so we don't reset animation every frame
        if (fightFullText != gameState.enemy->m_EnemyDialogue[gameState.Plot]) {
            FS_QueueFightText(gameState.enemy->m_EnemyDialogue[gameState.Plot]);
        }
    }

    // Render (animated) text and allow X to skip via the helper
    FS_UpdateAndRenderAnimatedText(renderer, font, { 255, 255, 255 }, event);

    // Press Z to advance but only after animation finished
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (!fightTextAnimating) {
            // after the player sees the intro text, select the first action.
            gameState.Plot++;
            gameState.fightState = FightState::PLAYER_TURN_MENU;
            selection = 0; // Reset selection for menu
        }

        //// If we've reached the end of intro dialogue, move to player's turn
        //if (gameState.Plot >= gameState.enemy->m_EnemyDialogue.size()) {
        //    gameState.fightState = FightState::PLAYER_TURN_MENU;
        //    selection = 0; // Reset selection for menu
        //}
    }
}

bool AttackMechanic(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
	// timing based attack mechanic
	// create a SDL_Rect for the target
	// create a SDL_Rect for the target area.
	// move the TargetRect across the screen.
    int randomSpeedMultiplier = chance(1, 2);
	gameState.FightTargetRect.x -= 5 * randomSpeedMultiplier; // move right by 10 pixels per frame, adjust speed as needed
    // render it
    FS_renderTextBox(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for target area
	SDL_RenderDrawRect(renderer, &gameState.FightTargetRect);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for target
	SDL_RenderDrawRect(renderer, &gameState.FightTargetAreaRect);
	// if the player presses Z when the TargetRect is inside the TargetAreaRect, return true.
    if (!gameState.FightAttackAttempt &&  event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z && event.key.repeat == 0) {
		// Only accept the initial keydown (not key-repeat events)
		gameState.FightAttackAttempt = true; // prevent multiple inputs
        SDL_Rect ActualArea = gameState.FightTargetAreaRect;
        ActualArea.w += 10;
        ActualArea.h += 10;
        if (SDL_HasIntersection(&gameState.FightTargetRect, &ActualArea)) {
			//printf("HIT!\n");
            return true; // Successful hit
        }
        else {
            return false; // Missed
		}
    }
	if (gameState.FightTargetRect.x < 0 || (gameState.FightTargetAreaRect.x - 20) > gameState.FightTargetRect.x ) {
		//FightState::RESULT_DIALOGUE;
        gameState.FightAttackAttempt = true;
        return false; // Out of bounds, treat as miss
    }
	return false; // No input yet
}


void HandleAttackMechanic(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {

    if (!gameState.fightTurnTimer.isStarted()) {
        gameState.fightTurnTimer.start();
        gameState.lastTurnTime = SDL_GetTicks();
        //printf("Attack TIME STARTED\n");
        //gameState.enemy->ResetProjectiles();
        // i want this line on fight start and only on fight start
        //gameState.player->m_HeartPos = { float(gameState.screenwidth / 2), float(gameState.screenheight / 2) }; // reset position
        //gameState.enemy->m_EnemyProjectile->m_Init = true; // reset projectile state
        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
        int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
        int yOffset = screenHeight - 275;       // Place the text inside the box
		gameState.FightTargetAreaRect = { xOffset + 200, yOffset + 100, 10, 40 }; // set target area position
        int randomXPositionMultiplier = chance(2, 4);
		gameState.FightTargetRect = { xOffset + 500, yOffset + 100, 10, 40 }; // set target position
		gameState.FightTargetRect.x = gameState.FightTargetAreaRect.x * randomXPositionMultiplier; // reset target position
		gameState.FightAttackAttempt = false; // reset attack attempt

    }
    else {
        if (SDL_GetTicks() - gameState.lastTurnTime >= gameState.turnTimeLimit) {
            // time's up, end turn
            gameState.fightTurnTimer.stop();
            //gameState.player->m_HeartVelocity = { 0,0 }; // stop fucking moving
            //gameState.fightState = FightState::ENEMY_TURN;

            //gameState.fightState = FightState::RESULT_DIALOGUE;
            //printf("Attack TIMER UP!\n");
            // Check if enemy defeated
            if (gameState.enemy->HP <= 0) {
                gameState.enemy->alive = false;
                //FS_QueueFightText("You defeated the enemy!");
                //gameState.kills++;
                //gameState.money += chance(10);
                //gameState.fightState = FightState::FIGHT_END;
                gameState.wonFight = true;
                gameState.fightState = FightState::FIGHT_END;
            }
            else {
                gameState.fightState = FightState::PLAYER_ACTION_RESULT /*FightState::PLAYER_ACTION_RESULT*/;
            }

        }
        else {
			// still in the attack mechanic.
            //if (gameState.FightAttackAttempt) {
            //    // already attempted attack, wait for turn to end
			//	gameState.lastTurnTime = gameState.turnTimeLimit; // fast forward to end
            //    return;
            //}
            if (AttackMechanic(renderer, font, event)) {
				printf("Attack HIT!\n");
                int damage = chance(10);
                if (damage == 1 || damage == 10) {
                    FS_QueueFightText("SMAAASHHH!!!!!!\n");
                    gameState.enemy->HP -= 2; // critical hit does double damage
                }
                else {
                    FS_QueueFightText("You hit the " + gameState.enemy->m_Name + "! \n");
                    gameState.enemy->HP -= 1; // normal hit
                }

                // If enemy died from this hit, record victory and go to result dialogue
                if (gameState.enemy->HP <= 0) {
                    gameState.enemy->alive = false;
                    gameState.kills++;
                    gameState.money += chance(10);
                    gameState.wonFight = true;
                    // Stop the turn and show the result dialogue
                    gameState.fightTurnTimer.stop();
                    gameState.FightAttackAttempt = true;
                    //gameState.fightState = FightState::RESULT_DIALOGUE;
					gameState.fightState = FightState::FIGHT_END;
                    return;
                }

                // Otherwise, end the turn and show the player's action result
                gameState.fightTurnTimer.stop();
                gameState.FightAttackAttempt = true;
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                return;
            }
            else if (gameState.FightAttackAttempt){
                //fightText = "You missed!!!\n";
                int miss = chance(8);
                if (miss >=3 && miss <=5) {
                    FS_QueueFightText("You missed!!!\n");
                }
                else {
                    FS_QueueFightText("Your attack was a little shallow. You hit the " + gameState.enemy->m_Name + " !\n");
                    gameState.enemy->HP -= 1; // Replace with proper damage calculation 
                }
                // End the turn immediately and show result so no further inputs apply this turn
                gameState.fightTurnTimer.stop();
                gameState.FightAttackAttempt = true;
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                return;
            }
            else { FS_QueueFightText("You Missed!"); }
        }
    }


}

void HandlePlayerTurnMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_renderTextBox(renderer);

    // Define menu options
    if (gameState.SillyMeter >= 5) {
        gameState.SillyMode = true;
        //
    }
    std::vector<std::string> fightMenu = { "Fight", "Actions", "Magic", "Items" };

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
				gameState.fightState = FightState::PLAYER_FIGHT;
				HandleAttackMechanic(renderer, font, event);
    //            // Attack enemy
    //            if (HandleAttackMechanic(renderer, font, event)) {
    //                gameState.enemy->HP -= 1; // Replace with proper damage calculation
    //                // Set text for result dialogue
    //                fightText = "You hit the " + std::string("{ENEMY ID: ") +
    //                    std::to_string(gameState.enemy->m_EnemyID) + " }\n";
    //            }
    //            else {
    //                fightText = "You missed!!!\n";
				//}

    //            

    //            // Check if enemy defeated
    //            if (gameState.enemy->HP <= 0) {
    //                gameState.enemy->alive = false;
    //                fightText = "You defeated the enemy!";
    //                gameState.kills++;
    //                gameState.money += chance(10);
    //                gameState.fightState = FightState::FIGHT_END;
    //            }
    //            else {
    //                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
    //            }
                break;

            case 1: // Actions
                gameState.fightState = FightState::PLAYER_ACTIONS_MENU;
                selection = 0; // Reset selection for new menu
                break;
            case 2: // Items
                gameState.fightState = FightState::PLAYER_MAGIC;
                selection = 0; // Reset selection for new menu
                break;
            case 3: // Items
                gameState.fightState = FightState::PLAYER_ITEMS_MENU;
                selection = 0; // Reset selection for new menu
                break;
            }
            gameState.turnCount++;
        }
    }
}


void HandlePlayerMagicMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // this is the menu for the actions the player can pick in a fight. 
    // They come from the enemy that started the fight. 

    FS_renderTextBox(renderer);

    // Get actions for current enemy
    //std::vector<std::string> actionMenu = { "Magic 1", "Magic 2"};
	//std::vector<Magic> *actionMenu = &gameState.player->m_Abilities;
	std::vector<std::unique_ptr<Magic>>* actionMenu = &gameState.player->m_Abilities;


    // Render menu options
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
    int xOffset = screenWidth * 0.05 + 30;
    int yOffset = screenHeight - 275;

    SDL_Color white = { 255,255,255 };
    SDL_Color red = { 237,28,36 };
    SDL_Color grey = { 128,128,128 };

    for (int i = 0; i < actionMenu->size(); i++) {
        SDL_Color color;
        if (i == selection) { color = red; }
        else if (gameState.TensionMeterCost[i] > gameState.TensionMeter) { color = grey; }
        else { color = white; }
        // SDL_Color color = (i == selection) ? red : white;
        FS_renderText(renderer, font, actionMenu->at(i)->m_abilityName, xOffset + (i * 300), yOffset, color);
    }

    // Handle input
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) {
            selection--;
            if (selection < 0) selection = actionMenu->size() - 1;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            selection++;
            if (selection >= actionMenu->size()) selection = 0;
        }
        else if (event.key.keysym.sym == SDLK_z) {
            // Set text based on selected action
            // The enemy will effect the gameState based on the action chosen here.
            if (gameState.TensionMeterCost[selection] > gameState.TensionMeter) {
                // player does not have enough Tension Points to cast this ability. 
                // I will play a sound to indicate that this option is unselectable. for now, do nothing. 
                return;
            }
            if (selection < actionMenu->size()) {
                //fightText = gameState.enemy->FightActionResponse(selection); // side effects handled inside the function
				FS_QueueFightText("Magic Menu: You used " + actionMenu->at(selection)->m_abilityName + "!\n");
                //gameState.TensionMeter -= gameState.TensionMeterCost[selection];
				gameState.TensionMeter -= actionMenu->at(selection)->m_TensionCost; // deduct the appropriate amount of tension points
				gameState.player->m_Abilities.at(selection)->Cast(); // call the Cast method of the selected ability, which will apply its effects to the game state
                //fightText = gameState.enemy->m_ActionResponse[selection]; // this is replaced by the above line for silly mode support
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                gameState.turnCount++;
                selection = 0; // Set to Actions option
            }
            else {
                FS_QueueFightText("ERROR: Invalid action!");
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


void HandlePlayerActionsMenuState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
	// this is the menu for the actions the player can pick in a fight. 
    // They come from the enemy that started the fight. 

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
			// The enemy will effect the gameState based on the action chosen here.

            if (selection < gameState.enemy->m_ActionResponse.size()) {
				FS_QueueFightText(gameState.enemy->FightActionResponse(selection)); // side effects handled inside the function
				//fightText = gameState.enemy->m_ActionResponse[selection]; // this is replaced by the above line for silly mode support
                gameState.fightState = FightState::PLAYER_ACTION_RESULT;
                gameState.turnCount++;
                selection = 0; // Set to Actions option
            }
            else {
                FS_QueueFightText("ERROR: Invalid action!");
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
            //itemMenu.push_back(GetItemnameFromIndex(gameState.Inventory[i]));
            itemMenu.push_back(gameState.Inventory[i]->m_ItemName);
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
			FS_QueueFightText(std::string("You used the ") + gameState.Inventory[selection]->m_ItemName); 
			//gameState.Inventory[selection]->Use(); // Use the selected item
            gameState.Inventory[selection]->Use();
            gameState.Inventory.erase(gameState.Inventory.begin() + selection);

            //selection = 2; // Set to Items option

			// Remove item from inventory
            //if (gameState.Inventory.size() > 0) {
            //    gameState.Inventory.erase(gameState.Inventory.begin() + selection);
            //}

            selection = 2; // Set to Items option
			gameState.fightState = FightState::PLAYER_ACTION_RESULT;
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
    FS_UpdateAndRenderAnimatedText(renderer, font, { 255, 255, 255 }, event);

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (!fightTextAnimating) {
            // Move to enemy's turn
            gameState.fightState = FightState::ENEMY_TURN;
        }
        // If text is animating, Z does nothing; X is the skip key
    }
}

void HandleFightEndState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event); // coupon clipping bastard
void HandleEnemyTurnState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    // Determine enemy action
	int action = chance(8); // A 1 in 8 chance for each action

    if (gameState.enemyID > 0 && gameState.enemyID < 5) {
        // Boss-fight specific logic would go here
        // For now, just move to dialogue
        gameState.fightState = FightState::ENEMY_DIALOGUE;
        return;
    }

    if (action >= 4 && action <= 6) { // a 3 in 8 chance for dialogue
        // Enemy speaks dialogue
        if (gameState.enemy->m_EnemyDialogue.size() > 0) {
            int dialogueIndex = chance(gameState.enemy->m_EnemyDialogue.size() - 1);
            FS_QueueFightText(gameState.enemy->m_EnemyDialogue[dialogueIndex]);
        }
        else {
            // Why is Enemy->m_EnemyDialogue empty..? Bad.
			FS_QueueFightText("The Enemy doesn't have m_EnemyDialogue populated. This is an Error.");
        }
        gameState.fightState = FightState::ENEMY_DIALOGUE;
    }
    else {
		// Otherwise, go to the dodge mechanic and check if the player dies.

        if (gameState.SillyMode) { FS_QueueFightText("You goofy ass trogladite..."); } // Current indicator of silly mode. 
        else { FS_QueueFightText(gameState.enemy->FightActionResponse(0)); }
        
   //     if (gameState.HP <= 0) { // this is the only way the player can die right now.
   //         gameState.HP = 0;
   //         fightText += "You were defeated!";
   //         FS_renderText(renderer, font, fightText, { 255, 255, 255 });
   //         gameState.fightState = FightState::FIGHT_END;
			//gameState.dead = true;
			//HandleFightEndState(renderer, font, event);
   //     }

        gameState.fightState = FightState::DODGE_MECHANIC;
    }
}

void HandleEnemyDialogueState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    FS_UpdateAndRenderAnimatedText(renderer, font, { 255, 255, 255 }, event);

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (!fightTextAnimating) {
            gameState.fightState = FightState::RESULT_DIALOGUE;
        }
    }
}

void HandleResultDialogueState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {

    FS_UpdateAndRenderAnimatedText(renderer, font, { 255, 255, 255 }, event);

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (!fightTextAnimating) {
    //        // Return to player's turn or end fight if that was final
    //        if ( /* gameState.wonFight || */ gameState.dead  /* || !gameState.enemy->alive */ || (gameState.enemy->HP <= 0)) {
				////EndFightAndReturnToFlow();
    //            gameState.fightState = FightState::FIGHT_END;
    //            return;
    //        }
            gameState.fightState = FightState::PLAYER_TURN_MENU;
        }
    }
}

void EndFightAndReturnToFlow() {
    // Reset fight state and return to main application flow
    gameState.inFight = false;
    gameState.wonFight = !gameState.dead;
    gameState.FightStarted = false;
    //gameState.dead = false;
    //gameState.fightState = FightState::INTRO;
    gameState.Plot = 0;
    selection = 0;
    FS_ClearFightTextQueue();
    if (!gameState.dead || !(gameState.HP <= 0) ) {
        //gameState.dead = true;
        int moneys = 0;
        if (gameState.TensionMeter > 0) moneys = chance(gameState.TensionMeter);
        gameState.money += moneys;
        gameState.Text.clear();
        std::string out = "You WON!! You got $" + std::to_string(moneys) + " for winning.\n";
        gameState.Text.push_back(out);
        gameState.textAvailable = true;
        gameState.shouldAnimateText = true;
        gameState.FightStarted = false;
        gameState.player->m_HeartVelocity = { 0,0 };
        gameState.player->m_VelX = 0; // stop fucking moving 
        gameState.player->m_VelY = 0;
    }
    else {
        gameState.Text = { "You lost the fight!" };
        return;

    }
}

void HandleFightEndState(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    if (gameState.HP <= 0 && !FightEndConsumed) { // this is the only way the player can die right now.
        gameState.HP = 0;
        fightText = "You were defeated!";
        FS_QueueFightText(fightText);
        fightShouldAnimateText = true;
        FightEndConsumed = true;

    }
    else {
        if (!FightEndConsumed) {
            printf("Handling fight end state. Dead: %d, Enemy Alive: %d, Enemy HP: %d\n", gameState.dead, gameState.enemy->alive, gameState.enemy->HP);
            fightText = "The Enemy Stopped Moving!";
            FS_QueueFightText(fightText);
            fightShouldAnimateText = true;
            FightEndConsumed = true;
        }
    }
    FS_UpdateAndRenderAnimatedText(renderer, font, { 255, 255, 255 }, event);
    //HandleResultDialogueState(renderer, font, event); // Reuse the result dialogue handler for end-of-fight summary and transition
    //FightEndConsumed = true; // Ensure this block only runs once
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
        if (!fightTextAnimating) {
            //// Return to player's turn or end fight if that was final
            //if ( /* gameState.wonFight || */ gameState.dead  /* || !gameState.enemy->alive */ || (gameState.enemy->HP <= 0)) {
            //    //EndFightAndReturnToFlow();
            //    gameState.fightState = FightState::FIGHT_END;
            //    return;
            //}
            //gameState.fightState = FightState::PLAYER_TURN_MENU;
            //FightEndConsumed = true;
            EndFightAndReturnToFlow();
        }
    }
        

    
    
	
 //   gameState.inFight = false;
 //   gameState.wonFight = !gameState.dead;
	//gameState.FightStarted = false;
	//
	//if (!gameState.dead) {
 //       int moneys = 0;
 //       if (gameState.TensionMeter > 0) moneys = chance(gameState.TensionMeter);
 //       gameState.money += moneys;
 //       gameState.Text.clear();
 //       std::string out = "You WON!! You got $" + std::to_string(moneys) + " for winning.\n";
 //       gameState.Text.push_back(out);
 //       gameState.textAvailable = true;
	//	gameState.shouldAnimateText = true;
	//	gameState.FightStarted = false;
	//	gameState.player->m_HeartVelocity = { 0,0 };
	//	gameState.player->m_VelX = 0; // stop fucking moving 
	//	gameState.player->m_VelY = 0;
	//}
    return; // game state has been notified of death. Return to Application Flow
	//else {
 //       //gameState.Text = { "You lost the fight!" };
 //        
	//	return;
	//}



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
	//printf("DODGE MECHANIC HERE\n");
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
        gameState.enemy->ResetProjectiles();
		// i want this line on fight start and only on fight start
		//gameState.player->m_HeartPos = { float(gameState.screenwidth / 2), float(gameState.screenheight / 2) }; // reset position
        //gameState.enemy->m_EnemyProjectile->m_Init = true; // reset projectile state
    }
    else {
        if (SDL_GetTicks() - gameState.lastTurnTime >= gameState.turnTimeLimit) {
            // time's up, end turn
            gameState.fightTurnTimer.stop();
            gameState.player->m_HeartVelocity = { 0,0 }; // stop fucking moving
            //gameState.fightState = FightState::ENEMY_TURN;

			//gameState.fightState = FightState::RESULT_DIALOGUE;
            gameState.fightState = FightState::PLAYER_TURN_MENU;
			printf("DODGE TIMER UP! FightState  is now Result_Dialogue\n");
            //if (gameState.HP <= 0) { // this is the only way the player can die right now.
            //    gameState.HP = 0;
            //   gameState.fightState = FightState::FIGHT_END;
            //}
		}
        else {
			// still the enemy's turn, update projectiles and handle player position.
			//printf("DODGE TIME ONGOING!\n");
            //printf("");
            // Update enemy projectile(s)
            if (gameState.HP <= 0) { // this is the only way the player can die right now.
                gameState.HP = 0;
                gameState.fightState = FightState::FIGHT_END;
            }
            gameState.enemy->m_EnemyProjectile->Update(gameState.deltaTime, gameState.player->m_HeartPos);
        }
    }
}



// Main fight system input handler. This is also how the fight system enters the Application Flow.
void FS_HandleInput(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    //gameState.player->m_HeartVelocity = { 0,0 }; // stop fucking moving
    //gameState.player->m_HeartPos = { 200.0f, 400.0f }; // reset position



    std::string enemyHP = "Enemy HP: " + std::to_string(gameState.enemy->HP);
    FS_renderText(renderer, font, enemyHP, 200, 50, { 237, 28, 36 });

    std::string Tension = "Tension: " + std::to_string(gameState.TensionMeter);
    FS_renderText(renderer, font, Tension, 600, 50, { 255, 255, 0 });

	// rolling HP meter logic
    // this needs to run once per actual second.
	if (gameState.DamageTaken > 0) {
        // If player took damage, animate HP decrease
        int displayedHP = std::max(gameState.HP, 0);
        std::string rollingHP = "HP: " + std::to_string(displayedHP);
        FS_renderText(renderer, font, rollingHP, 500, 50, { 237, 28, 36 });
        gameState.DamageTaken -= 1; // Decrease damage taken until it reaches 0
		gameState.HP -= 1; // Decrease actual HP to match displayed HP
    }
    else {
        // Always display HP regardless of state
        std::string HP = "HP: " + std::to_string(gameState.HP);
        FS_renderText(renderer, font, HP, 500, 50, { 237, 28, 36 });
    }

    if (gameState.enemy->HP <= 0) {
        gameState.enemy->alive = false;
        //FS_QueueFightText("You defeated the enemy!");
        //gameState.kills++;
        //gameState.money += chance(10);
        //gameState.fightState = FightState::FIGHT_END;
        gameState.wonFight = true;
        gameState.fightState = FightState::FIGHT_END;
    }
	//if (gameState.HP <= 0) {
 //       gameState.HP = 0;
 //       gameState.dead = true;
 //       gameState.fightState = FightState::FIGHT_END;
 //   }

    // Handle input and rendering based on current state
    switch (gameState.fightState) {
    case FightState::INTRO:
        HandleIntroState(renderer, font, event);
        break;

    case FightState::PLAYER_TURN_MENU:
        // This state expects immediate menu input; clear any lingering dialogue animation
        FS_ClearFightTextQueue();
        HandlePlayerTurnMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ACTIONS_MENU:
        FS_ClearFightTextQueue();
        HandlePlayerActionsMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ITEMS_MENU:
        FS_ClearFightTextQueue();
        HandlePlayerItemsMenuState(renderer, font, event);
        break;

    case FightState::PLAYER_ACTION_RESULT:
        HandlePlayerActionResultState(renderer, font, event);
        break;

    case FightState::ENEMY_TURN:
        FS_ClearFightTextQueue();
        HandleEnemyTurnState(renderer, font, event);
        break;
    case FightState::DODGE_MECHANIC:
        FS_ClearFightTextQueue();
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

    case FightState::PLAYER_FIGHT:
        // Handle the attack mechanic; do NOT clear fight text here so queued result text persists
        HandleAttackMechanic(renderer, font, event);
        break;

	case FightState::PLAYER_MAGIC:
		FS_ClearFightTextQueue();
		HandlePlayerMagicMenuState(renderer, font, event);
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
    // i want this line on fight start and only on fight start
    gameState.player->m_HeartPos = { float(gameState.screenwidth / 2), float(gameState.screenheight / 2) }; // reset position
	gameState.player->m_HeartVelocity = { 0,0 }; // stop fucking moving

    // Set initial state
    gameState.TensionMeter = 0;
    gameState.fightState = FightState::INTRO;

    // Initialize RNG if not already done
    if (!rngInitialized) {
        initRandom();
    }

    try {
        gameState.bgLayer1 = std::make_shared<BackgroundLayer>(gameState.fightLayer1);
        if (!gameState.bgLayer1->isValid()) {
            std::cerr << "Error: Failed to load layer1 (" << gameState.fightLayer1 << ")\n";
            //return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: Failed to load layer1 (" << gameState.fightLayer1 << "): " << e.what() << "\n";
        //return 1;
    }
    
    if (gameState.fightLayer2 != 0) {
        try {
            gameState.bgLayer2 = std::make_shared<BackgroundLayer>(gameState.fightLayer2);
            if (gameState.bgLayer2->isValid()) {
                gameState.alpha1 = 0.5f;
                gameState.alpha2 = 0.5f;
            } else {
                std::cerr << "Warning: Failed to load layer2 (" << gameState.fightLayer2 << ")\n";
                std::cerr << "Continuing with layer1 only\n";
                gameState.fightLayer2 = 0;
                gameState.alpha1 = 1.0f;
            }
        } catch (const std::exception &e) {
            std::cerr << "Warning: Failed to load layer2 (" << gameState.fightLayer2 << "): " << e.what() << "\n";
            std::cerr << "Continuing with layer1 only\n";
            gameState.fightLayer2 = 0;
            gameState.alpha1 = 1.0f;
        }
    }
}