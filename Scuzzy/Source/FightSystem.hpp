#ifndef FIGHTSYSTEM_HPP
#define FIGHTSYSTEM_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

#include "Source/GameState.hpp"
#include "Source/Enums.hpp"

void FS_renderTextBox(SDL_Renderer* renderer);
void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color);
void FS_renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color color);
void FS_HandleInput(SDL_Renderer* renderer, TTF_Font* font, SDL_Event event);
void FS_InitFight();
// Define the fight states


#endif
