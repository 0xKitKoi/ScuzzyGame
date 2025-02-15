#include <SDL.h>
#include <SDL_ttf.h>
#include < stdio.h >
#include <string>
#include <random>

#include "Source/GameState.hpp"

bool infightbutDialogue = false; // in a fight but the enemy is talking.
std::string fightText = "oopsie whoopsie i did a fuckywucky";


int chance(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd()); // Seed the random number generator once
    std::uniform_int_distribution<int> dist(1, n); // Generate numbers from 1 to n

    return dist(gen);
}

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

void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
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

// if (event.key.keysym.sym == SDLK_Z) {}

int selection = 0;
void FS_HandleInput(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event) {
    if (gameState.FightStarted) {
        if (event.key.keysym.sym == SDLK_z) {
            gameState.FightStarted = false;

            gameState.Plot++;
            gameState.TURN = true;
        }
        else {
            FS_renderText(renderer, font, gameState.enemy->m_EnemyDialogue[gameState.Plot], { 255, 255, 255 });
        }
        return;
    }


    if (gameState.TURN) {
        FS_renderTextBox(renderer);
        // render main fight menu and handle input. 
        std::vector<std::string> fightMenu = { "Fight", "Actions", "Items" };
        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
        int xOffset = screenWidth * 0.05 + 30;  // Start slightly inside the text box
        int yOffset = screenHeight - 275;       // Place the text inside the box

        for (int i = 0; i < fightMenu.size(); i++) {
            SDL_Color color = { 255, 255, 255 };
			if (i == selection) {
                color = { 237, 28, 36 };
			}
            if (i == 0) {
                FS_renderText(renderer, font, fightMenu[i], xOffset, yOffset, color);
            }
            else if (i == 1) {
                FS_renderText(renderer, font, fightMenu[i], xOffset + 300, yOffset, color);
            }
            else if (i == 2) {
                FS_renderText(renderer, font, fightMenu[i], xOffset + 600, yOffset, color);
			}
		}

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
				// Handle the action based on the selected menu item
				switch (selection) {
					case 0: // Fight
						printf("Fight\n");
                        gameState.TURN = false;
                        gameState.inFight = false;
                        gameState.textAvailable = true;
                        gameState.Text.push_back("You Won!");

						break;
					case 1: // Actions
						printf("Actions\n");
                        gameState.TURN = false;
						break;
					case 2: // Items
						printf("Items\n");
                        gameState.TURN = false;
						break;
				}
			}
		}
    }
    else {
        if (infightbutDialogue) {
            //FS_renderTextBox(renderer);
            FS_renderText(renderer, font, fightText, { 255, 255, 255 });
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_z) {
                    infightbutDialogue = false;
                    gameState.Plot++;
                    gameState.TURN = true;
                    /*
                    if (gameState.Plot >= gameState.enemy->m_EnemyDialogue.size()) {
                        gameState.Plot = 0;
                        infightbutDialogue = false;
                        gameState.TURN = true;
                    }
                    */
                }
            }
            return;
        }
        // since its not the player's turn, time to decide the fight mechanic. Undertale style dodging? cavestory fight in an undertale style format?
        // or just a simple fight system where you can choose to attack or use an item, and take the hit like in earthbound?
        // ... i'm thinking all three, but depending on the enemy.
        // I like how the main boss fights in undertale are all unique. normal fights will be like earthbound, but the main bosses
        // will have some spice. 
        if (gameState.enemyID > 0 && gameState.enemyID < 5) {
            // boss-fight fight mode. this will have dodge mechanics and special scripted things.
        }
        else {
            // this is a basic enemy type fight. Player will take some damage, or get dialogue.
            // FS_renderText(renderer, font, gameState.enemy->m_EnemyDialogue[gameState.Plot], { 255, 255, 255);
            int event = chance(5); // 50% chance to dodge or take damage
            if (event == 1) {
                // player takes damage
                gameState.HP -= gameState.enemy->m_AttackDamage;
                if (gameState.HP < 0) {
                    {
                        gameState.HP = 0; // impliment death ig
                        printf("You died!\n");

                    }
                    printf("You took %d damage!\n", gameState.enemy->m_AttackDamage);
                }
            }
            else if (event == 3 || event == 4) {
                    // Diaogue time
                    //gameState.Text.push_back(gameState.enemy->m_EnemyDialogue[gameState.Plot]);
                    //FS_renderText(renderer, font, gameState.enemy->m_EnemyDialogue[gameState.Plot], { 255, 255, 255 });
                    infightbutDialogue = true;
                    fightText = gameState.enemy->m_EnemyDialogue[ chance( gameState.enemy->m_EnemyDialogue.size() -1 ) ];
                    //gameState.Plot++;
            }
            else {
                    // player dodges
                    printf("You dodged the attack!\n");
                    infightbutDialogue = true;
                    fightText = "You dodged the attack!";
                    //FS_renderText(renderer, font, "You dodged the attack!", { 255, 255, 255 });
            }
        }
    }
}