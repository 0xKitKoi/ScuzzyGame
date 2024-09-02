/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>


#include "LTexture.hpp"
#include "player.hpp"
#include "Math.hpp"
#include "Timer.hpp"
#include <vector>
//#include <Timer.cpp>

//Screen dimension constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int LEVEL_WIDTH = 4000;
const int LEVEL_HEIGHT = 4000;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed PNG image
SDL_Surface* gPNGSurface = NULL;

//Globally used font
TTF_Font* gFont = NULL;

//Rendered TEXT texture
LTexture gTextTexture;

//Box collision detector
//bool checkCollision(SDL_Rect a, SDL_Rect b);

//The camera area
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };


//Scene sprites
//Walking animation
const int WALKING_ANIMATION_FRAMES = 20;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;
LTexture Map;


// GRID design for player collision detection.
// The idea behind this is to check collision's player's rect when they move.
// But we only want to check against collision boxes NEAR the player.
// this grid helps filter them down.
const int GRID_CELL_SIZE = 100;
const int GRID_WIDTH = LEVEL_WIDTH / GRID_CELL_SIZE;
const int GRID_HEIGHT = LEVEL_HEIGHT / GRID_CELL_SIZE;
std::vector<SDL_Rect> grid[GRID_WIDTH][GRID_HEIGHT];

std::vector<SDL_Rect> collisionBoxes;

// Black text color for use with gTextTexture;
SDL_Color textColor = { 0, 0, 0 };


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
			else
			{
				//Create renderer for window
				gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED ); // | SDL_RENDERER_PRESENTVSYNC  // enables vsync.
				if (gRenderer == NULL)
				{
					printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
					success = false;
				}
				else
				{
					//Initialize renderer color
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

					//Initialize PNG loading
					int imgFlags = IMG_INIT_PNG;
					if (!(IMG_Init(imgFlags) & imgFlags))
					{
						printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
						success = false;
					}
				}
			}
			//Initialize SDL_ttf
			if (TTF_Init() == -1)
			{
				printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
				success = false;
			}
		}
	}

	return success;
}

// "C:\\Users\\kirby\\OneDrive\\Desktop\\project gradiant.png"

void close()
{
	//	TODO: actually clean up after yourself.

	//Free loaded images
	SDL_FreeSurface(gPNGSurface);
	gPNGSurface = NULL;

	gSpriteSheetTexture.free();
	//gSpriteSheetTexture = NULL;

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}



bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		//SDL_Color textColor = { 0, 0, 0 };
		if (!gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}
	if (!Map.loadFromFile("concept art.bmp"))
	{
		printf("Failed to load sprite sheet texture!\n");
		success = false;
	}



	return success;
}

/*
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

void checkCollisions(Player& player) {
	int playerX = player.GetPosX() / GRID_CELL_SIZE;
	int playerY = player.GetPosY() / GRID_CELL_SIZE;

	for (int x = playerX - 1; x <= playerX + 1; ++x) {
		for (int y = playerY - 1; y <= playerY + 1; ++y) {
			if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
				for (const auto& box : grid[x][y]) {
					if (SDL_HasIntersection(&player.GetCollider(), &box)) {
						// Handle collision

					}
				}
			}
		}
	}
}
*/


void assignToGrid(const SDL_Rect& box) {
	// The idea of this is to only pass the vector of rects containing collision boxes to the player ONLY if the player is around them.
	// there's probably a better way to do this.

	int startX = box.x / GRID_CELL_SIZE;
	int startY = box.y / GRID_CELL_SIZE;
	int endX = (box.x + box.w) / GRID_CELL_SIZE;
	int endY = (box.y + box.h) / GRID_CELL_SIZE;

	// Make sure we stay within grid bounds
	startX = std::max(0, startX);
	startY = std::max(0, startY);
	endX = std::min(GRID_WIDTH - 1, endX);
	endY = std::min(GRID_HEIGHT - 1, endY);

	for (int x = startX; x <= endX; ++x) {
		for (int y = startY; y <= endY; ++y) {
			grid[x][y].push_back(box);
		}
	}
}

void initializeCollisionBoxes() {
	//	TODO: Level loading.
	// 
	// 
	// this is just a box made of collision boxes for player debugging. 
	// Left wall
	SDL_Rect leftWall;
	leftWall.x = 300;
	leftWall.y = 600;
	leftWall.w = 40;
	leftWall.h = 400;
	collisionBoxes.push_back(leftWall);

	// Top wall
	SDL_Rect topWall;
	topWall.x = leftWall.x;
	topWall.y = leftWall.y;
	topWall.w = leftWall.w + 300;
	topWall.h = 40;
	collisionBoxes.push_back(topWall);

	// Right wall
	SDL_Rect rightWall;
	rightWall.x = leftWall.x + 300;
	rightWall.y = leftWall.y;
	rightWall.w = leftWall.w;
	rightWall.h = leftWall.h;
	collisionBoxes.push_back(rightWall);

	// Bottom wall
	SDL_Rect bottomWall;
	bottomWall.x = leftWall.x;
	bottomWall.y = leftWall.y + leftWall.h - 40;
	bottomWall.w = topWall.w;
	bottomWall.h = 40;
	collisionBoxes.push_back(bottomWall);
}


void initializeCollisionGrid() {
	// The idea of this is to only pass the vector of rects containing collision boxes to the player ONLY if the player is around them.
	// there's probably a better way to do this.
	for (const auto& box : collisionBoxes) {
		assignToGrid(box);
	}
}


/*
void renderCollisionBoxes(SDL_Renderer* gRenderer, const std::vector<SDL_Rect>& collisionBoxes) {
	// Set the render draw color for walls (black in this case)
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	// Iterate through all collision boxes and draw them
	for (const auto& wall : collisionBoxes) {
		SDL_RenderDrawRect(gRenderer, &wall);
	}
}
*/

void renderCollisionBoxes(SDL_Renderer* gRenderer, const std::vector<SDL_Rect>& collisionBoxes, const SDL_Rect& camera) {
	// Set the render draw color for walls (black in this case)
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	// Iterate through all collision boxes and draw only if they are within the camera's viewport
	for (const auto& box : collisionBoxes) {
		// Calculate the intersection between the box and the camera
		SDL_Rect intersectedBox;
		if (SDL_IntersectRect(&box, &camera, &intersectedBox)) {
			// Adjust box position relative to camera
			SDL_Rect renderBox = {
				box.x - camera.x,
				box.y - camera.y,
				box.w,
				box.h
			};

			// Draw the box
			SDL_RenderDrawRect(gRenderer, &renderBox);
		}
	}
}


std::vector<SDL_Rect> getSurroundingCollisionBoxes(Player& player, const std::vector<SDL_Rect> grid[40][40]) {
	std::vector<SDL_Rect> surroundingBoxes;

	int playerX = player.GetCollider().x / GRID_CELL_SIZE;
	int playerY = player.GetCollider().y / GRID_CELL_SIZE;

	// Check the current and neighboring grid cells
	for (int x = std::max(0, playerX - 1); x <= std::min(GRID_WIDTH - 1, playerX + 1); ++x) {
		for (int y = std::max(0, playerY - 1); y <= std::min(GRID_HEIGHT - 1, playerY + 1); ++y) {
			surroundingBoxes.insert(surroundingBoxes.end(), grid[x][y].begin(), grid[x][y].end());
		}
	}

	return surroundingBoxes;
}



int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
		//Load media "C:\\Users\\kirby\\OneDrive\\Desktop\\project gradiant.png"
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			
			//The frames per second timer
			Timer fpsTimer;

			//The frames per second cap timer
			Timer capTimer;

			//In memory text stream
			std::stringstream timeText;

			//Start counting frames per second
			int countedFrames = 0;
			fpsTimer.start();

			 
			Vector2f playerinitpos;
			playerinitpos.x = 700;
			playerinitpos.y = 700;
			Player player(playerinitpos);
			
			

			// WALL
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 600;
			wall.w = 40;
			wall.h = 400;
			//assignToGrid(wall);

			//std::vector<SDL_Rect> collisionBoxes;

			initializeCollisionBoxes();
			initializeCollisionGrid();

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			Uint32 previousTime = 0; // Previous frame time
			Uint32 currentTime = 0; // Current frame time
			float deltaTime = 0.0f; // Time elapsed between frames


			Uint32 InitialTime = SDL_GetTicks();
			//Uint32 deltatime = 0;
			int frame = 0;
			//While application is running
			while (!quit)
			{
				//Start cap timer
				capTimer.start();

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					
					player.handleEvent(e);

				}

				//Calculate and correct fps
				float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				////Set text to be rendered
				//timeText.str("");
				//timeText << "Average Frames Per Second (With Cap) " << avgFPS;
				//if (!gTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
				//{
				//	printf("Unable to render FPS texture!\n");
				//}

				// Delta time
				currentTime = SDL_GetTicks();
				deltaTime = (currentTime - previousTime) / 1000.0f; // Convert to seconds
				previousTime = currentTime;

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);
				Map.render(0,0, &camera);

				gTextTexture.render(0,0); // render any text.
				//gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);

				// Get the collision boxes in surrounding cells
				std::vector<SDL_Rect> surroundingBoxes = getSurroundingCollisionBoxes(player, grid);

				// Pass the filtered collision boxes to the player
				player.Update(surroundingBoxes, deltaTime);
				//player.move(collisionBoxes);

				//Center the camera over the dot
				camera.x = (player.GetPosX() + player.SpriteWidth / 2) - SCREEN_WIDTH / 2;
				camera.y = (player.GetPosY() + player.SpriteHeight / 2) - SCREEN_HEIGHT / 2;

				//Keep the camera in bounds
				if (camera.x < 0)
				{
					camera.x = 0;
				}
				if (camera.y < 0)
				{
					camera.y = 0;
				}
				if (camera.x > LEVEL_WIDTH - camera.w)
				{
					camera.x = LEVEL_WIDTH - camera.w;
				}
				if (camera.y > LEVEL_HEIGHT - camera.h)
				{
					camera.y = LEVEL_HEIGHT - camera.h;
				}

				//player.Update(deltaTime);

				//Render wall
				//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
				//SDL_RenderDrawRect(gRenderer, &wall);
				renderCollisionBoxes(gRenderer, collisionBoxes, camera);


				player.render(camera.x,camera.y);

				//Update screen
				SDL_RenderPresent(gRenderer);
				++countedFrames;
				//Go to next animation frame for player
				++frame;

				//Cycle animation frames
				if (frame / 20 >= WALKING_ANIMATION_FRAMES)
				{
					frame = 0;
				}

				//If frame finished early
				int frameTicks = capTimer.getTicks();
				if (frameTicks < SCREEN_TICKS_PER_FRAME)
				{
					//Wait remaining time
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}