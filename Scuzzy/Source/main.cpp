#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
//#include <random>

#include "Source/LTexture.hpp"
#include "Source/Player.hpp"
#include "Source/Math.hpp"
#include "Source/Timer.hpp"
#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/NPC.hpp"
#include "Source/GameState.hpp"
#include "Source/MenuSystem.hpp"
#include "Source/FightSystem.hpp"
#include "Source/TestNPC.hpp"
#include "Helper.hpp"
#include "Source/Rom.h"
#include "Source/BackgroundLayer.h"

const std::string romPath = "data/truncated_backgrounds.dat";



constexpr int SNES_WIDTH  = 256;
constexpr int SNES_HEIGHT = 224;
constexpr int WINDOW_SCALE = 3;
constexpr int FPS = 30;
constexpr int FRAME_TIME_MS = 1000 / FPS;
int aspect_ratio = 16;

 std::vector<uint8_t> framebuffer(SNES_WIDTH * SNES_HEIGHT * 4);
 std::shared_ptr<BackgroundLayer> bgLayer1;
 std::shared_ptr<BackgroundLayer> bgLayer2;

 SDL_Texture* texture;


//Screen dimension constants
//const int SCREEN_WIDTH = 1920;
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;


Camera camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

//const int SCREEN_HEIGHT = 1080;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
const int LEVEL_WIDTH = 4000;
int levelWidth = 4000; // TODO: make the grid array a vector..? fix the math or abandon it?
const int LEVEL_HEIGHT = 4000;
int levelHeight = 4000;

int screenwidth = 0;
int screenheight = 0;
int MapoffsetX = 0, MapoffsetY = 0;
// Calculate centering offsets
int center_offset_x = 0;
int center_offset_y = 0;



//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed PNG image
SDL_Surface* gPNGSurface = NULL;

//Globally used font
TTF_Font* gFont = NULL;

//Rendered TEXT texture
LTexture gTextTexture;

//std::vector<std::shared_ptr<Entity>> Entities;
Vector2f playerinitpos;
SDL_Rect CheckBox;

//Box collision detector
//bool checkCollision(SDL_Rect a, SDL_Rect b);

//The camera area
//SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };


//Scene sprites
//Walking animation
const int WALKING_ANIMATION_FRAMES = 20;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;
LTexture Map;
LTexture deathScreen;

// GRID design for player collision detection.
// The idea behind this is to check collision's player's rect when they move.
// But we only want to check against collision boxes NEAR the player.
// this grid helps filter them down.
const int GRID_CELL_SIZE = 100;
const int GRID_WIDTH = LEVEL_WIDTH / GRID_CELL_SIZE;
const int GRID_HEIGHT = LEVEL_HEIGHT / GRID_CELL_SIZE;
std::vector<SDL_Rect> grid[GRID_WIDTH][GRID_HEIGHT];

std::vector<SDL_Rect*> collisionBoxes; // global because passing into funcs sounds horrible.
std::vector<SDL_Rect> staticCollisionBoxes; // boxes that dont belong to entities, like walls or invisible barriers.

// Black text color for use with gTextTexture;
//SDL_Color textColor = { 0, 0, 0 };
SDL_Color textColor = { 255, 0, 0 };

float lerp(float x, float y, float t) {
	return x * (1.f - t) + y * t;
}


GameState gameState;


std::vector<std::shared_ptr<Entity>> Entities; // This is the one and only vector of entities.


std::vector<SDL_Rect> clips; // sprite sheet mapings TODO: Doesnt need to be global, i was just lazy

std::unordered_map<std::string, std::shared_ptr<LTexture>> textureCache; 
// unfortunately needed because the pointer to my sprites were getting deleted when the LTexture went out of scope.
// getTexture("player.png") returns a pointer to the LTexture object, which is a wrapper around SDL_Texture*.
std::shared_ptr<LTexture> getTexture(const std::string& filename) {
	if (textureCache.find(filename) == textureCache.end()) { // If not found, make a LTexture object.
		auto texture = std::make_shared<LTexture>();
		if (!texture->loadFromFile(filename)) {
			std::cerr << "Failed to load texture: " << filename << std::endl;
			return nullptr;
		}
		textureCache[filename] = texture;
	}
	return textureCache[filename];
}

struct saveData {
	Vector2f pos;
	std::string room;
	std::vector<int> items;
	int kills = 0;
	int money = 0;
} SaveData;




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
		//SDL_GetCurrentDisplayMode(0, screenwidth, screenheight, NULL);
		SDL_DisplayMode displayMode;
		if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
    		printf("Could not get display mode: %s\n", SDL_GetError());
    		success = false;
    		// handle error, maybe return false or exit
		}else {
			screenwidth = (int)displayMode.w;
			screenheight = (int)displayMode.h;
			SCREEN_HEIGHT = screenheight;
			SCREEN_WIDTH = screenwidth;
			camera.width = SCREEN_WIDTH;
			camera.height = SCREEN_HEIGHT;

		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenwidth, screenheight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//camera.w = screenwidth;
			//camera.h = screenheight;
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
				//SDL_RenderSetLogicalSize(gRenderer, screenwidth, screenheight);

			}
			//Initialize SDL_ttf
			if (TTF_Init() == -1)
			{
				printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
				success = false;
			}
		}
	}


	//SDL_RenderSetLogicalSize(gRenderer, SNES_WIDTH, SNES_HEIGHT);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	texture = SDL_CreateTexture(
		gRenderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SNES_WIDTH,
		SNES_HEIGHT
	);

	if (!ROM::loadFromFile(romPath)) {
		std::cerr << "Failed to load ROM: " << romPath << "\n";
    	return 1;
	}

	return success;
}


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

	// clear vectors:
	Entities.clear();
	collisionBoxes.clear();
	staticCollisionBoxes.clear();

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
	return loadedSurface;
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

void GameStart() {
	Entities.clear();
	collisionBoxes.clear();
	playerinitpos = SaveData.pos;
	CheckBox = { (int)playerinitpos.x,(int)playerinitpos.y, 20,20};
	gameState.room = SaveData.room;
	//gameState.Inventory = PopulateInventory(SaveData.items);
	PopulateInventory(SaveData.items);
	gameState.kills = SaveData.kills;
	gameState.money = SaveData.money;
	gameState.textAvailable = false;
	gameState.dead = false;
	//gameState.CurrentSave = SaveData;
	//gameState.HP = 10;

	LoadLevel(gameState.room, &Map);
	
}






void SaveGame(int x, int y) {
	printf("Saving.\n");
	std::ofstream saveFile("save");
	if (saveFile.is_open()) {
		SaveData.pos.x = x;
		SaveData.pos.y = y;
		SaveData.room = gameState.room;
		//SaveData.items = gameState.Inventory;
		SaveInventory(SaveData.items);
		saveFile << SaveData.pos.x << " " << SaveData.pos.y << "\n";
		saveFile << SaveData.room << "\n";
		saveFile << SaveData.kills << "\n";
		saveFile << SaveData.money << "\n";
		saveFile << gameState.HP << "\n";
		
		saveFile << SaveData.items.size();
		if (SaveData.items.size() > 0) {
			for (int i = 0; i < SaveData.items.size(); i++) {
				saveFile << SaveData.items.at(i) << " ";
			}
			saveFile << "\n";
			saveFile.close();
		}
	}
	else {
		printf("Couldn't save.");
	}
	return;
}

int LoadSave() {
    std::ifstream saveFile("save");
    if (saveFile.is_open()) {
        // Check if reads are successful
        if (!(saveFile >> SaveData.pos.x >> SaveData.pos.y)) {
            printf("Error reading position data.\n");
            saveFile.close();
            return 0;
        }
        saveFile.ignore();
        
        if (!(saveFile >> SaveData.room)) {
            printf("Error reading room data.\n");
            saveFile.close();
            return 0;
        }
        saveFile.ignore();
        
        if (!(saveFile >> SaveData.kills)) {
            printf("Error reading kills data.\n");
            saveFile.close();
            return 0;
        }
        saveFile.ignore();
        
        if (!(saveFile >> SaveData.money)) {
            printf("Error reading money data.\n");
            saveFile.close();
            return 0;
        }
        saveFile.ignore();
        
        if (!(saveFile >> gameState.HP)) {
            printf("Error reading HP data.\n");
            saveFile.close();
            return 0;
        }
        saveFile.ignore();
        
        int items;
        if (!(saveFile >> items)) {
            printf("Error reading items count.\n");
            saveFile.close();
            return 0;
        }
        
        SaveData.items.resize(items);
        for (int i = 0; i < items; i++) {
            if (!(saveFile >> SaveData.items[i])) {
                printf("Error reading item %d.\n", i);
                saveFile.close();
                return 0;
            }
        }
        saveFile.close();
		//gameState.CurrentSave = SaveData;
        return 1; // Success
    }
    else {
        printf("Could not read save data.\n");
        SaveData.pos.x = 700;
        SaveData.pos.y = 700;
        SaveData.room = "test";
        gameState.room = "test";
        SaveData.items.clear();
        SaveData.kills = 0;
        SaveData.money = 0;
        gameState.HP = 10;
        SaveData.items.push_back(0);
        SaveGame(SaveData.pos.x, SaveData.pos.y);
        return 0;
    }
}


/*
int LoadSave() {
	std::ifstream saveFile("save");
	if (saveFile.is_open()) {
		saveFile >> SaveData.pos.x >> SaveData.pos.y;
		saveFile.ignore(); // ignore \n;
		saveFile >> SaveData.room;
		saveFile.ignore(); // ignore \n;
		saveFile >> SaveData.kills;
		saveFile.ignore();
		saveFile >> SaveData.money;
		saveFile.ignore();
		saveFile >> gameState.HP;
		saveFile.ignore();

		int items;
		saveFile >> items;
		SaveData.items.resize(items);
		for (int i = 0; i < items; i++) {
			saveFile >> SaveData.items[i];
		}
		saveFile.close();
	}
	else {
		printf("Could not read save data.");
		SaveData.pos.x = 700;
		SaveData.pos.y = 700;
		SaveData.room = "test";
		gameState.room = "test";
		SaveData.items.clear();
		SaveData.kills = 0;
		SaveData.money = 0;
		gameState.HP = 10;
		SaveData.items.push_back(0); // add healing item
		SaveGame(SaveData.pos.x, SaveData.pos.y); // save the default save.
		return 0;
	}

	return 1;
}
*/

void handleDeath(SDL_Event e, Player player) {
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	// render the death png
	// auto barrel = std::make_shared<Entity>(Vector2f(2322, 258), SDL_Rect{ 0,0,128,128 }, SDL_Rect{ 0,0,128,128 }, getTexture("data/barrel_nuclear.png"), 1, clips, 1);
	//Entities.clear();
	SDL_RenderClear(gRenderer);
	deathScreen.render(screenwidth / 2, screenheight / 2);
	//SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, count);
	//SDL_RenderClear(gRenderer);


	SDL_RenderPresent(gRenderer);
	if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_z) {
		// reset the game
		LoadSave();
		GameStart();
		gameState.dead = false;
		/*main(argc, args);*/ // lmfao dont call main in main lesson learned
	}
}



bool loadMedia()
{

	//Loading success flag
	bool success = true;

	//Open the font
	gFont = TTF_OpenFont("data/lazy.ttf", 28);
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

	/*
	int ls = LoadSave();
	if (ls) {
		if (SaveData.room == "test") {
			if (!Map.loadFromFile("data/concept art.bmp")) // "data/concept art.bmp" data/startingalley.png
			{
				printf("Failed to load sprite sheet texture!\n");
				success = false;
			}
		}
		if (SaveData.room == "Level1") {
			if (!Map.loadFromFile("data/fight convepts.png"))
			{
				printf("Failed to load sprite sheet texture!\n");
				success = false;
			}
			std::string levelstats = "LEVEL STATS: ";
			levelstats += std::to_string(Map.getHeight());
			levelstats += "x";
			levelstats += std::to_string(Map.getWidth());
			printf(levelstats.c_str());
		}
		if (SaveData.room == "Level2") {
			if (!Map.loadFromFile("data/startingalley.png"))
			{
				printf("Failed to load sprite sheet texture!\n");
				success = false;
			}
			SaveData.pos.x = 100;
			SaveData.pos.y = 100;
			SaveData.room = "Level2";
			std::string levelstats = "LEVEL STATS: ";
			levelstats += std::to_string(Map.getHeight());
			levelstats += "x";
			levelstats += std::to_string(Map.getWidth());
			printf(levelstats.c_str());
		}
	}
	else {
		// Couldn't load save data. Assume game is starting for first time.
		if (!Map.loadFromFile("data/Error.png"))
		{
			printf("Failed to load sprite sheet texture!\n");
			success = false;
		}
		//SaveData.pos.x = 700;
		//SaveData.pos.y = 700;
		//SaveData.room = "test";
	}
	*/
	LoadSave();
	LoadLevel(SaveData.room, &Map);
	levelHeight = Map.getHeight();
	levelWidth = Map.getWidth();
	deathScreen.loadFromFile("data/Death.png");

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

SDL_Rect leftWall;
SDL_Rect topWall;
SDL_Rect rightWall;
SDL_Rect bottomWall;

void initializeCollisionBoxes() {
	//	TODO: Level loading.
	// 
	// 
	// this is just a box made of collision boxes for player debugging. 
	// Left wall
	//SDL_Rect leftWall;
	leftWall.x = 300;
	leftWall.y = 600;
	leftWall.w = 40;
	leftWall.h = 400;
	collisionBoxes.push_back(&leftWall);

	// Top wall
	//SDL_Rect topWall;
	topWall.x = leftWall.x;
	topWall.y = leftWall.y;
	topWall.w = leftWall.w + 300;
	topWall.h = 40;
	collisionBoxes.push_back(&topWall);

	// Right wall
	//SDL_Rect rightWall;
	rightWall.x = leftWall.x + 300;
	rightWall.y = leftWall.y;
	rightWall.w = leftWall.w;
	rightWall.h = leftWall.h;
	collisionBoxes.push_back(&rightWall);

	// Bottom wall
	//SDL_Rect bottomWall;
	bottomWall.x = leftWall.x;
	bottomWall.y = leftWall.y + leftWall.h - 40;
	bottomWall.w = topWall.w;
	bottomWall.h = 40;
	collisionBoxes.push_back(&bottomWall);
}


void initializeCollisionGrid() {
	// The idea of this is to only pass the vector of rects containing collision boxes to the player ONLY if the player is around them.
	// there's probably a better way to do this.
	for (const auto& box : collisionBoxes) {
		assignToGrid(*box);
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

//void renderCollisionBoxes(SDL_Renderer* gRenderer, const std::vector<SDL_Rect>& collisionBoxes, const SDL_Rect& camera) {
void renderCollisionBoxes(SDL_Renderer* gRenderer, const SDL_Rect& camera) {
	// Set the render draw color for walls (black in this case)
	//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	// Iterate through all collision boxes and draw only if they are within the camera's viewport
	for ( auto& box : collisionBoxes) { // const?
		// Calculate the intersection between the box and the camera
		SDL_Rect intersectedBox;
		if (SDL_IntersectRect( box, &camera, &intersectedBox)) {
			// Adjust box position relative to camera
			SDL_Rect renderBox = {
				box->x - camera.x,
				box->y - camera.y,
				box->w,
				box->h
			};

			// Draw the box
			SDL_RenderDrawRect(gRenderer, &renderBox);
		}
	}
	for (const auto& box : staticCollisionBoxes) {
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


/*
void renderTextBox(SDL_Renderer* renderer, int x, int y, int width, int height) {
	SDL_Rect box = { x, y, width, height };

	// Render black background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &box);

	// Render deep red border
	// #660010
	SDL_SetRenderDrawColor(renderer, 237, 28, 36, 255);
	SDL_RenderDrawRect(renderer, &box);
}*/




void renderTextBox(SDL_Renderer* renderer) {
	int boxWidth = screenwidth * 0.9;  // 90% of the screen width
	int boxHeight = 300;               // Fixed height for the text box
	int xPos = (screenwidth - boxWidth) / 2;  // Center the box horizontally
	int yPos = screenheight- boxHeight - 20; // Place the box 20 pixels above the bottom

	// Set the color for the text box (e.g., black)
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect textBoxRect = { xPos, yPos, boxWidth, boxHeight };
	SDL_RenderFillRect(renderer, &textBoxRect);

	// Draw a red border around the text box
	SDL_SetRenderDrawColor(renderer, 103, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &textBoxRect);
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect dstRect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, NULL, &dstRect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

//int dialogueIndex = 0;
/*
std::vector<std::string> dialogue = {
	"Hello, welcome to the game.",
	"We hope you enjoy your journey.",
	"Press Z to continue."
};*/
void handleDialogue(SDL_Event event) {

	// ok this could be the sign NPC or the Merchant.
	if (gameState.callbackNPC != nullptr) {
		// this is not the sign NPC.
	}


	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) {
		// If we don't have any valid text, cancel the dialog state
		if (gameState.Text.empty() || gameState.textIndex < 0 || static_cast<size_t>(gameState.textIndex) >= gameState.Text.size()) {
			gameState.textAvailable = false;
			return;
		}
		if (gameState.shouldAnimateText && gameState.textAnimating) {
			// If text is still animating, show the full text immediately
			gameState.currentDisplayText = gameState.Text[gameState.textIndex];
			gameState.textAnimating = false;
		}
		else if (gameState.textIndex < gameState.Text.size() - 1) {
			// Move to next line and start animating if needed
			gameState.textIndex++;
			if (gameState.shouldAnimateText) {
				gameState.currentCharIndex = 0;
				gameState.textTimer = 0.0f;
				gameState.textAnimating = true;
				gameState.currentDisplayText = "";
			}
		}
		else {
			// Dialogue finished
			gameState.textIndex = 0;
			gameState.textAvailable = false;
			gameState.textAnimating = false;
		}
	}
}


void renderDialogue(SDL_Renderer* renderer, TTF_Font* font) {
	// Get screen dimensions
	int screenWidth, screenHeight;
	SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

	// Render the text box at the bottom of the screen
	renderTextBox(renderer);

	// Calculate text position inside the text box
	int boxWidth = screenWidth * 0.9;
	int xOffset = screenWidth * 0.05 + 20;  // Small margin inside the box
	int yOffset = screenHeight - 275;       // Positioning inside the text box

	// Ensure we have valid text to display
	if (gameState.Text.empty() || gameState.textIndex < 0 || static_cast<size_t>(gameState.textIndex) >= gameState.Text.size()) {
		// Nothing to render
		return;
	}

	// If the current line exists, update and render it
	if (gameState.currentCharIndex < gameState.Text.at(gameState.textIndex).size()) {
		// Update text animation if needed
		if (gameState.shouldAnimateText && gameState.textAnimating) {
			gameState.textTimer += 1.0f / 60.0f;//60.0f; // Assuming 60 FPS
			if (gameState.textTimer >= gameState.textSpeed) {
				gameState.textTimer = 0.0f;
				if (gameState.currentCharIndex < gameState.Text.at(gameState.textIndex).length()) {
					gameState.currentDisplayText += gameState.Text.at(gameState.textIndex).at(gameState.currentCharIndex);
					gameState.currentCharIndex++;
				}
				else {
					gameState.textAnimating = false;
				}
			}
		}


	}
	// Render the current text
	SDL_Color white = { 255, 255, 255 };  // Normal text color
	if (gameState.shouldAnimateText && gameState.textAnimating) {
		renderText(renderer, font, gameState.currentDisplayText, xOffset, yOffset, white);
	}
	else {
		renderText(renderer, font, gameState.Text[gameState.textIndex], xOffset, yOffset, white);
	}
}



std::vector<std::string> menuOptions = {
	"Start Game",
	"Options",
	"Exit"
};
int selectedIndex = 0;

void renderMenu(SDL_Renderer* renderer, TTF_Font* font) {
	renderTextBox(renderer);
	SDL_Color white = { 255, 255, 255 };
	SDL_Color red = { 179, 0, 0 };

	for (int i = 0; i < menuOptions.size(); i++) {
		SDL_Color color = (i == selectedIndex) ? red : white;
		renderText(renderer, font, menuOptions[i], 60, 410 + (i * 40), color);
	}
}

void handleMenuInput(SDL_Event event) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_UP) {
			selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : menuOptions.size() - 1;
		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			selectedIndex = (selectedIndex < menuOptions.size() - 1) ? selectedIndex + 1 : 0;
		}
		else if (event.key.keysym.sym == SDLK_z) {
			// Perform action based on selectedIndex
			if (selectedIndex == 0) {
				// Start Game
				gameState.inMenu = false;
			}
			else if (selectedIndex == 1) {
				// Options
				gameState.inMenu = false;
			}
			else if (selectedIndex == 2) {
				// Exit
				gameState.inMenu = false;
			}
		}
	}
}



void renderMenuSideBySide(SDL_Renderer* renderer, TTF_Font* font) {
	// Get screen dimensions
	int screenWidth = SCREEN_WIDTH, screenHeight = SCREEN_HEIGHT;
	SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

	// Render the text box at the bottom of the screen
	renderTextBox(renderer);

	// Calculate text rendering position and dynamic spacing
	int boxWidth = screenWidth * 0.9;
	int xOffset = screenWidth * 0.05 + 10;  // Start slightly inside the text box
	int yOffset = screenHeight - 275;       // Place the text inside the box

	int totalTextWidth = 0;
	//int numOptions = menuOptions.size();
	int numOptions = gameState.Text.size();

	// Clamp selectedIndex to valid range for these options
	if (numOptions > 0 && (selectedIndex < 0 || selectedIndex >= numOptions)) {
		selectedIndex = 0;
	}

	// Calculate total width of all the text
	for (const auto& option : gameState.Text) {
		//totalTextWidth += getTextWidth(font, option);
		int textWidth = 0;
		int textHeight = 0;
		TTF_SizeText(font, option.c_str(), &textWidth, &textHeight);
		totalTextWidth += textWidth;
	}

	// Calculate remaining space and distribute it as spacing between options
	int remainingSpace = boxWidth - totalTextWidth;
	int spacing = remainingSpace / (numOptions + 1);

	// Render each option side by side with dynamic spacing
	int currentX = xOffset + spacing;

	SDL_Color white = { 255, 255, 255 };  // Normal text color
	SDL_Color red = { 237, 28, 36 };   // Highlighted text color

	for (int i = 0; i < numOptions; i++) {
		SDL_Color color = (i == selectedIndex) ? red : white;

		renderText(renderer, font, gameState.Text[i], currentX, yOffset, color);

		//currentX += getTextWidth(font, menuOptions[i]) + spacing;
		int textWidth = 0;
		int textHeight = 0;
		TTF_SizeText(font, gameState.Text[i].c_str(), &textWidth, &textHeight);
		currentX += textWidth + spacing;
	}
}

void handleMenuInputSideBySide(SDL_Event event) {
	// If there is no text to show, ignore input
	if (gameState.Text.empty()) return;

	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			// Move left, wrapping around if necessary
			selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : static_cast<int>(gameState.Text.size()) - 1;
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			// Move right, wrapping around if necessary
			selectedIndex = (selectedIndex < static_cast<int>(gameState.Text.size()) - 1) ? selectedIndex + 1 : 0;
		}
		else if (event.key.keysym.sym == SDLK_z) {
			// Perform action based on selected option
			/*
			if (selectedIndex == 0) {
				// Start Game
				gameState.inMenu = false;
			}
			else if (selectedIndex == 1) {
				// Options
				gameState.inMenu = false;
			}
			else if (selectedIndex == 2) {
				// Exit
				gameState.inMenu = false;
			}
			*/
			gameState.selectionIndex = selectedIndex + 1; // 0 is not selected anything yet!
			gameState.inMenu = false;
			if (gameState.callbackNPC) {
				gameState.callbackNPC->handleChoice(gameState.selectionIndex); // tell the NPC who triggered a choice the selection.
			}
			
		}
	}
}



// Camera calculation with centering
void update_camera(int player_x, int player_y, int map_width, int map_height) {
    // Start with player-centered camera
    camera.x = player_x - screenwidth / 2;
    camera.y = player_y - screenheight / 2;
    
    // Apply boundaries for maps larger than screen
    if (map_width > screenwidth) {
        // Normal camera clamping for wide maps
        if (camera.x < 0) camera.x = 0;
        if (camera.x > map_width - screenwidth) {
            camera.x = map_width - screenwidth;
        }
    } else {
        // Center the map horizontally
        camera.x = -center_offset_x;
    }
    
    if (map_height > screenheight) {
        // Normal camera clamping for tall maps
        if (camera.y < 0) camera.y = 0;
        if (camera.y > map_height - screenheight) {
            camera.y = map_height - screenheight;
        }
    } else {
        // Center the map vertically
        camera.y = -center_offset_y;
    }
}






// Rendering function
void render_map(SDL_Renderer* renderer , LTexture& map_texture) {
	int map_width = map_texture.getWidth();
	int map_height = map_texture.getHeight();
    SDL_Rect src_rect = {0, 0, map_width, map_height};
    SDL_Rect dest_rect;
    
    if (map_width < screenwidth && map_height < screenheight) {
        // Map is smaller than screen in both dimensions - center it
        dest_rect.x = center_offset_x;
        dest_rect.y = center_offset_y;
        dest_rect.w = map_width;
        dest_rect.h = map_height;
        
        SDL_RenderCopy(renderer, map_texture.getTexture(), &src_rect, &dest_rect);

    } else {
        // Map is larger in at least one dimension - use camera
        if (map_width > screenwidth || map_height > screenheight) {
            // Calculate what part of the map to show
            src_rect.x = (camera.x > 0) ? camera.x : 0;
            src_rect.y = (camera.y > 0) ? camera.y : 0;
            src_rect.w = (map_width > screenwidth) ? screenwidth : map_width;
            src_rect.h = (map_height > screenheight) ? screenheight : map_height;
        }
        
        // Destination is screen position with centering offsets
        dest_rect.x = (map_width < screenwidth) ? center_offset_x : ((camera.x < 0) ? -camera.x : 0);
        dest_rect.y = (map_height < screenheight) ? center_offset_y : ((camera.y < 0) ? -camera.y : 0);
        dest_rect.w = src_rect.w;
        dest_rect.h = src_rect.h;
        
        SDL_RenderCopy(renderer, map_texture.getTexture(), &src_rect, &dest_rect);
    }
}

// Alternative simpler approach - unified rendering
void render_map_unified(SDL_Renderer* renderer, LTexture* map_texture) {
    SDL_Rect src_rect, dest_rect;
	int map_width = map_texture->getWidth();
	int map_height = map_texture->getHeight();
    // Calculate source rectangle (what part of map to draw)
    if (map_width <= screenwidth) {
        src_rect.x = 0;
        src_rect.w = map_width;
        dest_rect.x = center_offset_x;
        dest_rect.w = map_width;
    } else {
        src_rect.x = camera.x;
        src_rect.w = screenwidth;
        dest_rect.x = 0;
        dest_rect.w = screenwidth;
    }
    
    if (map_height <= screenheight) {
        src_rect.y = 0;
        src_rect.h = map_height;
        dest_rect.y = center_offset_y;
        dest_rect.h = map_height;
    } else {
        src_rect.y = camera.y;
        src_rect.h = screenheight;
        dest_rect.y = 0;
        dest_rect.h = screenheight;
    }
    
    SDL_RenderCopy(renderer, map_texture->getTexture(), &src_rect, &dest_rect);
}




int main(int argc, char* args[])
{
	//srand((unsigned)time(NULL));
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


			GameStart();
			
			//std::vector<std::shared_ptr<Entity>> Entities; // made global because everything needs to see all entities for collision detection.
			//std::unique_ptr<Player> player = std::make_unique<Player>(playerinitpos, Entities);
			Player player(playerinitpos, Entities);
			gameState.player = &player;
			LTexture heart;
			heart.loadFromFile("data/HeartSpriteSheet.png");
			player.m_FightSpriteSheet = heart;

			
			/*
			    Parent parent;

				// Create a Child and pass the parent to it
				std::shared_ptr<Child> child = std::make_shared<Child>(parent);

				// Set the child in the parent
				parent.setChild(std::move(child));

				// Call the child's function through the parent
				parent.callChildFunction();
			*/



			// WALL
			SDL_Rect wall = { 300, 600, 40, 400 };
			//assignToGrid(wall);

			//std::vector<SDL_Rect> collisionBoxes;

			//initializeCollisionBoxes();
			//initializeCollisionGrid();

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			Uint32 previousTime = 0; // Previous frame time
			Uint32 currentTime = 0; // Current frame time
			float deltaTime = 0.0f; // Time elapsed between frames

			gameState.screenheight = screenheight;
			gameState.screenwidth = screenwidth;

			Uint32 InitialTime = SDL_GetTicks();
			int frame = 0;



			/*
			 __    __     ______     __     __   __   
			/\ "-./  \   /\  __ \   /\ \   /\ "-.\ \  
			\ \ \-./\ \  \ \  __ \  \ \ \  \ \ \-.  \ 
			 \ \_\ \ \_\  \ \_\ \_\  \ \_\  \ \_\\"\_\
			  \/_/  \/_/   \/_/\/_/   \/_/   \/_/ \/_/
                                          
			 __         ______     ______     ______  
			/\ \       /\  __ \   /\  __ \   /\  == \ 
			\ \ \____  \ \ \/\ \  \ \ \/\ \  \ \  _-/ 
			 \ \_____\  \ \_____\  \ \_____\  \ \_\   
			  \/_____/   \/_____/   \/_____/   \/_/   
			
			*/
			
			int windowWidth, windowHeight;
			SDL_GetWindowSize(gWindow, &windowWidth, &windowHeight);
			printf("Window size: %d x %d\n", windowWidth, windowHeight);
			camera.width = windowWidth;
			camera.height = windowHeight;

			uint32_t tick = 0;
    		uint32_t frameCount = 0;

			while (!quit)
			{
				//Start cap timer
				capTimer.start();
				uint32_t frameStart = SDL_GetTicks();


				gameState.deltaTime = deltaTime;

				

				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}


					switch (e.type) {
						/* Look for a keypress */
					case SDL_KEYDOWN:
						/* Check the SDLKey values and move change the coords */
						switch (e.key.keysym.sym) {
						case SDLK_F10:
							SaveGame(player.GetPosX(), player.GetPosY());
							break;
						case SDLK_F11:
							LoadSave();
							break;
						case SDLK_F4:
							gameState.Text = {
								// 100 char limit per line
								"AAAA BBBB CCCC DDDD EEEE FFFF GGGG HHHH IIII JJJJ KKKK LLLL MMMM NNNN OOOO PPPP QQQQ RRRR SSSS TTTT UUUU VVVV WWWW XXXX"
							};
							gameState.textIndex = 0;
							gameState.currentCharIndex = 0;
							gameState.textTimer = 0.0f;
							gameState.textAnimating = true;
							gameState.currentDisplayText = gameState.Text[0][0];
							gameState.shouldAnimateText = true;  // This is dialogue, so animate it
							gameState.textAvailable = true;
							break;
						case SDLK_F5:
							gameState.Text = { "Start", "Options", "Quit" };
							gameState.textIndex = 0;
							gameState.shouldAnimateText = false;  // This is a menu, so don't animate
							gameState.inMenu = true;
							break;
						case SDLK_c:
							gameState.inMenu = true;
							gameState.shouldAnimateText = false;
							MS_renderMenu(gRenderer, gFont);
							break;
						default:
							break;
						}
						break;
						/* We must also use the SDL_KEYUP events to zero the x */
						/* and y velocity variables. But we must also be       */
						/* careful not to zero the velocities when we shouldn't*/
					case SDL_KEYUP:
						switch (e.key.keysym.sym) {
						default:
							break;
						}
						break;

					default:
						break;
					}

					/*if (e.key.keysym.scancode == SDL_SCANCODE_F10) {
						SaveGame(player.GetPosX(), player.GetPosY());
					}*/
					/*if (e.key.keysym.scancode == SDL_SCANCODE_F11) {
						LoadSave();
					}*/

					//if (e.key.keysym.scancode == SDL_SCANCODE_F4) {
					//	/*
					//	gameState.Text = {
					//		"Hello, welcome to the game.",
					//		"We hope you enjoy your journey.",
					//		"Press Z to continue."
					//		};
					//	*/
					//	gameState.Text = {
					//		// 100 char limit per line
					//		"AAAA BBBB CCCC DDDD EEEE FFFF GGGG HHHH IIII JJJJ KKKK LLLL MMMM NNNN OOOO PPPP QQQQ RRRR SSSS TTTT UUUU VVVV WWWW XXXX"
					//	};

					//	gameState.textAvailable = true;
					//}
					//if (e.key.keysym.scancode == SDL_SCANCODE_F5) {
					//	gameState.Text = { "Start", "Options", "Quit" };
					//	gameState.inMenu = true;
					//}
					//if (e.key.keysym.scancode == SDL_SCANCODE_C) {
					//	// opejn a menu
					//	/*
					//	gameState.Text = { "Talk", "Items" };
					//	gameState.inMenu = true;
					//	gameState.OpenedMenu = true;
					//	*/

					//	gameState.inMenu = true;

					//	MS_renderMenu(gRenderer, gFont);
					//	//SDL_RenderPresent(gRenderer);
					//}


					//if (gameState.dead) {
					//	// render the death png
					//	// auto barrel = std::make_shared<Entity>(Vector2f(2322, 258), SDL_Rect{ 0,0,128,128 }, SDL_Rect{ 0,0,128,128 }, getTexture("data/barrel_nuclear.png"), 1, clips, 1);
					//	Entities.clear();
					//	LTexture deathScreen;
					//	deathScreen.loadFromFile("data/Death.png");
					//	deathScreen.render(0, 0, &camera);
					//	continue;

					//}


					if (gameState.dead) {
							SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
							// render the death png
							// auto barrel = std::make_shared<Entity>(Vector2f(2322, 258), SDL_Rect{ 0,0,128,128 }, SDL_Rect{ 0,0,128,128 }, getTexture("data/barrel_nuclear.png"), 1, clips, 1);
							//Entities.clear();
							SDL_RenderClear(gRenderer);
							deathScreen.render(screenwidth / 2, screenheight / 2);
							//SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, count);
							//SDL_RenderClear(gRenderer);


							SDL_RenderPresent(gRenderer);
							if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_z) {
								// reset the game
								printf("reseting the game\n");
								LoadSave();
								//player.reset(new Player(SaveData.pos, Entities));
								player.reset(SaveData.pos);
								player.SetPosX(SaveData.pos.x);
								player.SetPosY(SaveData.pos.y);
								player.currentState = State::Idle; // why is the little bastard moving on his own 
								player.AllEntities = Entities;
								//player = std::make_unique<Player>(SaveData.pos, Entities);
								GameStart();
								gameState.dead = false;
								gameState.textAvailable = false;
								levelHeight = Map.getHeight(); // needed to make sure death upon smaller maps doesnt break rendering loop
								levelWidth = Map.getWidth();
								gameState.HP = 10;

								/*main(argc, args);*/ // lmfao dont call main in main lesson learned
							}
					}
					else {
						if (gameState.textAvailable) {
							// either text or an NPC!
							/*
								if (gameState.callbackNPC != nullptr) {
								printf("Handling NPC dialogue\n");
								// This is an NPC dialogue.
								//gameState.callbackNPC->handleDialogue(e);
								// render and handle the dialogue options.
								// menu side by side
								renderMenuSideBySide(gRenderer, gFont);
								handleMenuInputSideBySide(e);
								
							}
							*/
							if (gameState.OpenedMenu) {
								// this is a menu
								//renderMenuSideBySide(gRenderer, gFont);
								handleMenuInputSideBySide(e);
							}
							else {
								handleDialogue(e);
							}
							
						}

						else if (gameState.inMenu) {
							//handleMenuInput(e);
							//handleMenuInputSideBySide(e);
							MS_renderMenu(gRenderer, gFont);
							MS_handleMenuInput(e);
							gameState.player->reset({ float(gameState.player->m_PosX), float(gameState.player->m_PosY) }); // fix player stuck issue

						}
						
						else if (gameState.inFight) {

							FS_HandleInput(gRenderer, gFont, e); // Give control to FightSystem
							player.handleEvent(e, deltaTime); // player heart controls
						}
						else {
							player.handleEvent(e, deltaTime); // normal overworld controls
						}
						
					}
				}

				//Calculate and correct fps
				float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
				if (avgFPS > 2000000)
				{
					avgFPS = 0;
				}

				//// Debug text
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
				gameState.deltaTime = deltaTime;


				// Starting Point!

				if (gameState.dead) {
					//handleDeath(e, player);
					// dont bother with the rest of the application loop.
					continue;
				}

				if (gameState.LoadingScreen) {
					// fade to black, load map texture based on room name, and fade back in.
					// player position and Level is loaded from the DOOR.
					// Maybe player direction should be given..?
					while (!gameState.DoneLoading) { // fade OUT
						Vector2f balls[255];
						for (int i = 0; i < 255; i++) {
							balls[i].x = rand() % SCREEN_WIDTH;
							balls[i].y = rand() % SCREEN_HEIGHT;
						}
						int count = 0;
						while (count < 255) {
							draw_circle(gRenderer, balls[count].x, balls[count].y, 120, { 0, 0, 20, 0xFF });
							SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, count);
							//SDL_RenderClear(gRenderer);
							SDL_RenderPresent(gRenderer);
							count += 1;
						}

						// when done, load the new map, and fade back in.
						Vector2f mapdim = LoadLevel(gameState.room, &Map);
						levelHeight = mapdim.y;
						levelWidth = mapdim.x;
						gameState.DoneLoading = true;
					}
					while (gameState.fade) { // fade IN

						int offsetX = 0, offsetY = 0;
						//Center the camera
						//camera.x = (player.GetPosX() + player.SpriteWidth / 2) - screenwidth / 2;
						//camera.y = (player.GetPosY() + player.SpriteHeight / 2) - screenheight / 2;
						camera.centerOn(player.GetPosX(), player.GetPosY());

						/*
						//Keep the camera in bounds
						if (camera.x < 0)
						{
							camera.x = 0;
						}
						if (camera.y < 0)
						{
							camera.y = 0;
						}
						if (camera.x > levelWidth - camera.w) // LEVEL_WIDTH
						{
							camera.x = levelWidth - camera.w;
						}
						if (camera.y > levelHeight - camera.h) // LEVEL_HEIGHT
						{
							camera.y = levelHeight - camera.h;
						}

						camera.w = std::min(levelWidth, screenwidth);  // Don't exceed level size
						camera.h = std::min(levelHeight, screenheight);
						*/

						/*
						//camera.x = std::max(0, std::min(player.GetPosX() - camera.w / 2, levelWidth - camera.w));
						//camera.y = std::max(0, std::min(player.GetPosY() - camera.h / 2, levelHeight - camera.h));
						if (levelWidth > windowWidth || levelHeight > windowHeight) {
							camera.x = std::max(0, std::min(player.GetPosX() - camera.w / 2, levelWidth - camera.w));
							camera.y = std::max(0, std::min(player.GetPosY() - camera.h / 2, levelHeight - camera.h));
						} else {
							camera.x = 0;
							camera.y = 0;
						}*/

						Uint8 a = 0;
						int count = 255;
						Map.setAlpha(a);
						player.CurrentSprite.setAlpha(a);
						while (count > 0) { // fade level in
							//Cap if over 255
							if (a + 32 > 255)
							{
								a = 255;
								gameState.fade = false;
							}
							//Increment otherwise
							else
							{
								a += 32;
							}
							//Render front blended
							Map.setAlpha(a);
							//Map.render(offsetX, offsetY, &camera);
							SDL_Rect mapScreenPos = camera.worldToScreen({0, 0, Map.getWidth(), Map.getHeight()});
							SDL_Rect what = {camera.x, camera.y, camera.width, camera.height};
    						SDL_RenderCopy(gRenderer, Map.getTexture(), &what, &mapScreenPos);

							player.CurrentSprite.setAlpha(a);
							player.render(camera.x, camera.y); // i need to render the player in the right spot relative to the camera. 
							SDL_RenderPresent(gRenderer);
							count -= 1;
						}
						gameState.DoneLoading = true;
					}

					// the new map is loaded, and the player is in the right spot. now stop loading..?
					gameState.LoadingScreen = false;
					//initializeCollisionBoxes();

				}

				if (!gameState.inFight) {	// OverWorld Rendering 

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 20, 0xFF);
					SDL_RenderClear(gRenderer);
					//int offsetX = 0, offsetY = 0;
					//camera.centerOn(player.GetPosX(), player.GetPosY());
					//camera.centerOn(player.GetPosX() - camera.x, player.GetPosY() - camera.y);
					//camera.centerOn(player.GetPosX(), player.GetPosY());
					//    camera.centerOn(player.GetPosX() + player.SpriteWidth / 2.0f,
                    //player.GetPosY() + player.SpriteHeight / 2.0f);

					//if (windowHeight > levelHeight) {
					//	//MapoffsetX = (windowWidth - levelWidth) / 2;
					//	MapoffsetY = (windowHeight - levelHeight) / 2;
					//}
					//else {MapoffsetX = 0;}
					//if (windowWidth > levelWidth) {
					//	MapoffsetX = (windowWidth - levelWidth) / 2;
					//	
					//}
					//else {MapoffsetY = 0;}

					//Map.render(MapoffsetX, MapoffsetY, &camera);


					MapoffsetY = 0;
					MapoffsetX = 0;
					bool smol = false;
					//if (windowHeight > levelHeight) {
					//	//MapoffsetX = (windowWidth - levelWidth) / 2;
					//	MapoffsetY = (windowHeight - levelHeight) / 2;
					//	smol = true;
					//}
					//if (windowWidth > levelWidth) {
					//	MapoffsetX = (windowWidth - levelWidth) / 2;
					//	smol = true;

					//}
					//if (smol) {
					//	Map.render(MapoffsetX, MapoffsetY);
					//}
					//else {
					//	Map.render(MapoffsetX, MapoffsetY, &camera);
					//}

					/*
					float scaleX = (float)windowWidth / levelWidth;
					float scaleY = (float)(windowHeight - (windowHeight * 0.2f)) / levelHeight; // Reserve 20% of the height for padding
					float scale = std::min(scaleX, scaleY); // Use the smaller scale to maintain aspect ratio
					scale = std::min(scale, 1.0f); // Prevent scaling above 100%

					int scaledWidth = levelWidth * scale;
					int scaledHeight = levelHeight * scale;

					int horizontalPadding = (windowWidth - scaledWidth) / 2;
					int verticalPadding = ((windowHeight - scaledHeight) / 2 );

					MapoffsetX = horizontalPadding;
					MapoffsetY = std::max(0, verticalPadding); // Ensure it doesn't go negative

					if (levelWidth > windowWidth || levelHeight > windowHeight) {
    					Map.render(0, 0, &camera, scale); // Use camera for larger maps
					} else {
    					Map.render(MapoffsetX, MapoffsetY, nullptr, scale); // Center smaller maps with padding
					}

					printf("MapoffsetX: %d, MapoffsetY: %d\n", MapoffsetX, MapoffsetY);
					printf("scaledWidth: %d, scaledHeight: %d\n", scaledWidth, scaledHeight);
					*/
					center_offset_x = 0;
					center_offset_y = 0;
					// If map is smaller than screen, center it
					if (Map.getWidth() < screenwidth) {
    					center_offset_x = (screenwidth - Map.getWidth()) / 2;
					}

					if (Map.getHeight() < screenheight) {
    					center_offset_y = (screenheight - Map.getHeight()) / 2;
					}
					gameState.MapoffsetX = center_offset_x;
					gameState.MapoffsetY = center_offset_y;
					gameState.levelHeight = Map.getHeight();
					gameState.levelWidth = Map.getWidth();
					//MapoffsetX = center_offset_x;
					//MapoffsetY = center_offset_y;
					//////update_camera(player.GetPosX(), player.GetPosY(), Map.getWidth(), Map.getHeight());
					//render_map(gRenderer, Map);
					//////////////////render_map_unified(gRenderer, &Map);

					// Render map
    				//SDL_Rect mapScreenPos = camera.worldToScreen({0, 0, Map.getWidth(), Map.getHeight()});
    				//SDL_RenderCopy(gRenderer, Map.getTexture(), NULL, &mapScreenPos);




					// Which part of the map texture to show (source rectangle)
					SDL_Rect srcRect = {
						(int)std::max(0.0f, camera.x),
						(int)std::max(0.0f, camera.y),
						std::min(camera.width, Map.getWidth()),
						std::min(camera.height, Map.getHeight())
					};
					
					// Where to draw it on screen (destination rectangle)
					SDL_Rect dstRect = {
						(camera.x < 0) ? (int)-camera.x : 0,  // Offset if map is smaller
						(camera.y < 0) ? (int)-camera.y : 0,
						srcRect.w,
						srcRect.h
					};
					
					SDL_RenderCopy(gRenderer, Map.getTexture(), &srcRect, &dstRect);	

					/*
					    printf("Camera: (%.1f, %.1f) | Player: (%d, %d)\n", 
           camera.x, camera.y, player.m_PosX, player.m_PosY);
							printf("Map Offset: (%d, %d)\n", center_offset_x, center_offset_y);
							printf("SrcRect: (%d, %d, %d, %d)\n", srcRect.x, srcRect.y, srcRect.w, srcRect.h);
							printf("DstRect: (%d, %d, %d, %d)\n", dstRect.x, dstRect.y, dstRect.w, dstRect.h);
					*/







					gTextTexture.render(0, 0); // render any text.
					//gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);

					// Get the collision boxes in surrounding cells
					//std::vector<SDL_Rect> surroundingBoxes = getSurroundingCollisionBoxes(player, grid);

					// Pass the filtered collision boxes to the player
					//player.Update(surroundingBoxes, deltaTime);
					//player.handleEvent(e); // player heart controls
					player.Update(collisionBoxes, deltaTime);


					

					//SDL_RenderDrawRect(gRenderer, &player.GetColliderAddress());
					SDL_Rect renderBox = {
						player.GetCollider().x - camera.x,
						player.GetCollider().y - camera.y,
						player.GetCollider().w,
						player.GetCollider().h
					};
					// Draw the player's collision box
					//SDL_RenderDrawRect(gRenderer, &renderBox);
					//player.move(collisionBoxes);

					//Center the camera over the dot
					//camera.x = (player.GetPosX() + player.SpriteWidth / 2) - screenwidth / 2;
					//camera.y = (player.GetPosY() + player.SpriteHeight / 2) - screenheight / 2;

					/*
					//Keep the camera in bounds
					if (camera.x < 0)
					{
						camera.x = 0;
					}
					if (camera.y < 0)
					{
						camera.y = 0;
					}
					if (camera.x > levelWidth - camera.w) // LEVEL_WIDTH
					{
						camera.x = levelWidth - camera.w;
					}
					if (camera.y > levelHeight - camera.h) // LEVEL_HEIGHT
					{
						camera.y = levelHeight - camera.h;
					}
					*/


					// TESTING MAP OFFSETS


					//camera.w = std::min(levelWidth, windowWidth);  // Don't exceed level size
					//camera.h = std::min(levelHeight, windowHeight);

					//camera.x = std::max(0, std::min(player.GetPosX() - camera.w / 2, levelWidth - camera.w));
					//camera.y = std::max(0, std::min(player.GetPosY() - camera.h / 2, levelHeight - camera.h));

					
					// Calculate map offsets for centering smaller maps
					//if (windowWidth > levelWidth) {
					//	MapoffsetX = (windowWidth - levelWidth) / 2;
					//}
					//else {
					//	MapoffsetX = 0;
					//}

					//if (windowHeight > levelHeight) {
					//	MapoffsetY = (windowHeight - levelHeight) / 2;
					//}
					//else {
					//	MapoffsetY = 0;
					//}


					// Center the camera over the player
					/*
					camera.w = std::min(levelWidth, windowWidth);  // Don't exceed level size
					camera.h = std::min(levelHeight, windowHeight);
					*/


					/*
					// Only apply camera if the map is larger than the screen
					if (levelWidth > windowWidth) {
						camera.x = std::max(0, std::min(player.GetPosX() + player.SpriteWidth / 2 - windowWidth / 2,
							levelWidth - camera.w));
					}
					else {
						camera.x = 0; // No camera movement needed for small maps
					}

					if (levelHeight > windowHeight) {
						camera.y = std::max(0, std::min(player.GetPosY() + player.SpriteHeight / 2 - windowHeight / 2,
							levelHeight - camera.h));
					}
					else {
						camera.y = 0; // No camera movement needed for small maps
					}*/

					/*
					if (levelWidth > windowWidth || levelHeight > windowHeight) {
						camera.x = std::max(0, std::min(player.GetPosX() + player.SpriteWidth / 2 - windowWidth / 2, levelWidth - camera.w));
						camera.y = std::max(0, std::min(player.GetPosY() + player.SpriteHeight / 2 - windowHeight / 2, levelHeight - camera.h));
					} else {
						camera.x = 0;
						camera.y = 0;
					}*/


					//player.Update(deltaTime);

					//Render wall
					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					//SDL_RenderDrawRect(gRenderer, &wall);



					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					//renderCollisionBoxes(gRenderer, camera);
					//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);


					for (auto& box : Entities) { // const?
						if (box->m_Enemy != NULL) {
							if (!box->m_Enemy->alive) {
								continue;
							}
						}
						// Calculate the intersection between the box and the camera
						SDL_Rect intersectedBox;
						SDL_Rect cameraRect = { camera.x, camera.y, camera.width, camera.height };
						if (SDL_IntersectRect(&box->m_FOV, &cameraRect, &intersectedBox)) {
							// Adjust box position relative to camera
							SDL_Rect renderBox = {
								box->m_FOV.x - cameraRect.x,
								box->m_FOV.y - cameraRect.y,
								box->m_FOV.w,
								box->m_FOV.h 
							};

							// Draw the box
							SDL_RenderDrawRect(gRenderer, &renderBox);
						}
					}

					renderCollisionBoxes(gRenderer, {int(camera.x), int(camera.y), camera.width, camera.height});
					//SDL_RenderDrawRect(gRenderer, &player.m_CheckBox);

					//player.render(camera.x, camera.y); // moved so player renders above everything else. might have to come back to this.



					//collisionBoxes.push_back(player.GetCollider());
					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					// Update every entity.
					for (int i = 0; i < Entities.size(); i++) {
						if (Entities[i]->m_Enemy != NULL) {
							if (!Entities[i]->m_Enemy->alive) {
								
								
								for (int j = 0; j < collisionBoxes.size(); j++) {
									if (collisionBoxes[j] == &Entities[i]->m_Collider) {
										// remove the collision box from the list.
										collisionBoxes.erase(collisionBoxes.begin() + j);
									}
								}
								//Entities[i].~shared_ptr();
								continue;
							}
						}
						/*if (gameState.inMenu) { break; }*/
						Entities.at(i)->Update(deltaTime, camera, player.GetCollider());

						//SDL_RenderDrawRect(gRenderer, &Entities.at(i).m_FOV);

						// render collision boxes
						SDL_Rect intersectedBox;
						SDL_Rect cameraRect = { camera.x, camera.y, camera.width, camera.height };
						if (SDL_IntersectRect(&Entities.at(i)->m_Collider, &cameraRect, &intersectedBox)) {
							//printf("bruh");
							// Adjust box position relative to camera
							SDL_Rect renderBox = {
								Entities.at(i)->m_Collider.x - camera.x + MapoffsetX,
								Entities.at(i)->m_Collider.y - camera.y + MapoffsetY,
								Entities.at(i)->m_Collider.w,
								Entities.at(i)->m_Collider.h
							};
							// comment this out soon
							renderBox = camera.worldToScreen(Entities.at(i)->m_Collider);

							// Draw the box
							SDL_RenderDrawRect(gRenderer, &renderBox);



						}

					}

					{
						// RENDER BASIC CHECK BOX
						SDL_Rect checkb = player.m_CheckBox;
						SDL_Rect rendercheckBox = {
							checkb.x - camera.x,
							checkb.y - camera.y,
							checkb.w,
							checkb.h
						};
						rendercheckBox = camera.worldToScreen(player.m_CheckBox);

						// Draw the box
						SDL_RenderDrawRect(gRenderer, &rendercheckBox);
					}

					//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);



					//Cycle animation frames
					++countedFrames;
					//Go to next animation frame for player
					++frame;

					if (frame / 20 >= WALKING_ANIMATION_FRAMES)
					{
						frame = 0;
					}


					//printf("Final MapoffsetX: %d, MapoffsetY: %d\n", MapoffsetX, MapoffsetY);
					//printf("Final scaledWidth: %d, scaledHeight: %d\n", scaledWidth, scaledHeight);

					// END OF OVERWORLD RENDERING 
					//player.render(camera.x + MapoffsetX, camera.y + MapoffsetY); // last thing to be rendered is the player so it's above everything else.

					/*
					if (levelWidth > windowWidth || levelHeight > windowHeight) {
						player.render(camera.x + MapoffsetX, camera.y + MapoffsetY); // Use camera for larger maps
					} else {
						player.render(MapoffsetX, MapoffsetY); // Use offsets for smaller maps
					}*/
					player.render(camera.x, camera.y); // Use camera for larger maps


					SDL_RenderDrawRect(gRenderer, &renderBox); // render players collision box above player.


					//SDL_RenderClear(renderer);



					//SDL_RenderPresent(renderer);



					if (gameState.textAvailable) {
						renderDialogue(gRenderer, gFont);
						player.currentState = State::Idle;
						player.reset({float(player.GetPosX()), float(player.GetPosY())}); // reset the player position to the current position, so it doesn't move while dialogue is active.
					}
					else if (gameState.OpenedMenu) {
						renderMenuSideBySide(gRenderer, gFont);
						player.currentState = State::Idle;
						player.reset({ float(player.GetPosX()), float(player.GetPosY()) });

					}
					else if (gameState.inMenu) {
						//renderMenuSideBySide(gRenderer, gFont);
						MS_renderMenu(gRenderer, gFont);
						player.currentState = State::Idle;
						player.reset({ float(player.GetPosX()), float(player.GetPosY()) });
					}

					if (gameState.checkFlag) {
						bool someone = false;
						player.currentState = State::Idle;
						player.reset({ float(player.GetPosX()), float(player.GetPosY()) });
						for (const auto& entity : Entities) {
							if (SDL_HasIntersection(&player.m_CheckBox, &entity->m_Collider)) { // &entity->m_Collider
								printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
								printf(std::to_string(entity->m_EntityID).c_str());
								someone = true;
								if (entity->m_NPC) {
									entity->m_NPC->m_checked = true;
								}
							}
						}
						if (!someone) {
							gameState.Text.clear();
							gameState.Text.push_back("Who are you talking to..?");
							gameState.textIndex = 0;
							gameState.currentCharIndex = 1; // offset because i need a char to start the animation.
							gameState.textTimer = 0.0f;
							gameState.textAnimating = true;
							gameState.currentDisplayText = gameState.Text.at(0).substr(0, 1); // "";
							gameState.shouldAnimateText = true;  // This is dialogue, so animate it
							gameState.textAvailable = true;
						}
						gameState.checkFlag = false;
					}



					//Update screen
					SDL_RenderPresent(gRenderer);

				}
				else { // IN FIGHT 
					//Clear screen
					//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

					/*
					SDL_UpdateTexture(texture, nullptr, framebuffer.data(), SNES_WIDTH * 4);
        			SDL_RenderClear(gRenderer);
        			SDL_RenderCopy(gRenderer, texture, nullptr, nullptr);
        			SDL_RenderPresent(gRenderer);
					*/


					//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					//SDL_RenderClear(gRenderer);
					//SDL_RenderCopy(gRenderer, texture, nullptr, nullptr); // battle background inserted here
					//SDL_UpdateTexture(texture, nullptr, framebuffer.data(), SNES_WIDTH * 4);
        			SDL_RenderClear(gRenderer);
        			SDL_RenderCopy(gRenderer, texture, nullptr, nullptr);
        			//SDL_RenderPresent(gRenderer);
					std::string bruh;
					bruh.append(std::to_string(gameState.enemyID));
					gTextTexture.loadFromRenderedText(bruh, { 0xFF, 0xFF, 0xFF, 0xFF });
					//gTextTexture.loadFromRenderedText(bruh, { 0x00, 0x00, 0x00, 0x00 });
					gTextTexture.render(0, 0);

					// Enemy will now render itself like a man
					gameState.enemy->Update(deltaTime, SCREEN_HEIGHT, SCREEN_WIDTH);


					/*
					try {
        				bgLayer1 = std::make_shared<BackgroundLayer>(gameState.fightLayer1);
        				if (!bgLayer1->isValid()) {
            				std::cerr << "Error: Failed to load layer1 (" << gameState.fightLayer1 << ")\n";
            				return 1;
        				}
    				} catch (const std::exception &e) {
        				std::cerr << "Error: Failed to load layer1 (" << gameState.fightLayer1 << "): " << e.what() << "\n";
        				return 1;
    				}

    				// Set alpha blending based on whether we have 2 layers
    				float alpha1 = 1.0f;
    				float alpha2 = 0.0f;
    
    				if (gameState.fightLayer2 != 0) {
        				try {
            				bgLayer2 = std::make_shared<BackgroundLayer>(gameState.fightLayer2);
            				if (bgLayer2->isValid()) {
                				alpha1 = 0.5f;
                				alpha2 = 0.5f;
            				} else {
                				std::cerr << "Warning: Failed to load layer2 (" << gameState.fightLayer2 << ")\n";
                				std::cerr << "Continuing with layer1 only\n";
                				gameState.fightLayer2 = 0;
                				alpha1 = 1.0f;
            				}
        				} catch (const std::exception &e) {
            					std::cerr << "Warning: Failed to load layer2 (" << gameState.fightLayer2 << "): " << e.what() << "\n";
            					std::cerr << "Continuing with layer1 only\n";
            					gameState.fightLayer2 = 0;
            					alpha1 = 1.0f;
        				}
    				}
					*/

        			// Clear framebuffer
        			memset(framebuffer.data(), 0, framebuffer.size());

       	 			// Apply layer 1 distortion (without letterbox yet, erase=true for first layer)
        			gameState.bgLayer1->overlayFrame(framebuffer.data(), SNES_WIDTH, SNES_HEIGHT, 0, 
                               static_cast<float>(tick), gameState.alpha1, true);

        			// Apply layer 2 distortion if present (erase=false to blend)
        			if (gameState.bgLayer2 && gameState.bgLayer2->isValid()) {
            			gameState.bgLayer2->overlayFrame(framebuffer.data(), SNES_WIDTH, SNES_HEIGHT, 0, 
                                   static_cast<float>(tick), gameState.alpha2, false);
        			}

        			// Apply letterbox AFTER distortion (just fill the edge pixels)
        			if (aspect_ratio > 0) {
            			for (int y = 0; y < aspect_ratio; ++y) {
                			for (int x = 0; x < SNES_WIDTH; ++x) {
                    			// Top letterbox
                    			int idx = (y * SNES_WIDTH + x) * 4;
                    			framebuffer[idx + 0] = 0;      // B
                    			framebuffer[idx + 1] = 0;      // G
                    			framebuffer[idx + 2] = 0;      // R
                    			framebuffer[idx + 3] = 255;    // A
                    
                    			// Bottom letterbox
                    			idx = ((SNES_HEIGHT - 1 - y) * SNES_WIDTH + x) * 4;
                    			framebuffer[idx + 0] = 0;      // B
                    			framebuffer[idx + 1] = 0;      // G
                    			framebuffer[idx + 2] = 0;      // R
                    			framebuffer[idx + 3] = 255;    // A
                			}
            			}
        			}


        			SDL_UpdateTexture(texture, nullptr, framebuffer.data(), SNES_WIDTH * 4);
        			//SDL_RenderClear(gRenderer);
        			//SDL_RenderCopy(gRenderer, texture, nullptr, nullptr);
        			//SDL_RenderPresent(gRenderer);

        			tick += 1; // frameskip set to 1 for now
        			frameCount++;


        			uint32_t frameTime = SDL_GetTicks() - frameStart;
        			//if (frameTime < FRAME_TIME_MS) {
            		//	SDL_Delay(FRAME_TIME_MS - frameTime);
        			//}
					/* currectly:
						int frameTicks = capTimer.getTicks();
						if (frameTicks < SCREEN_TICKS_PER_FRAME)
						{
							//Wait remaining time
							SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
						}
					*/


					

					// Get this from Enemy ID.
					//gameState.enemy->m_EnemyFightSpriteSheet->render((SCREEN_WIDTH/2)-128, (SCREEN_HEIGHT/2)-128*2, &rect);
					//std::vector <SDL_Rect *> boundaries;
					//SDL_Rect test = { 0, 0, SCREEN_WIDTH, 100 };
					//boundaries.push_back(&test); // top TEST BOUNDARY
					// so i cant call these here, it HAS to be where SDL Polls for events to populate e. checks. 
					//player.handleEvent(e); // player heart controls
					//FS_HandleInput(gRenderer, gFont, e); // Give control to FightSystem
					//
					//
					// NO it dies have to be here as well as in the event polling loop to catch all events. otherwise the heart is not rendered..? wtfff
					// the application loop right now is this:
					/*
					main()
						while not quit
							while poll event
								handle event for player
								handle event for fightsystem
							if not in fight
								render overworld, etc
							else
								HERE!
								update enemy
								update player input func ?
								hand over to Fight System State Machine ? (SDL_Event e is an argument)
								render fight system
					
					*/ 					

					player.handleEvent(e, deltaTime);
					FS_HandleInput(gRenderer, gFont, e); // Give control to FightSystem

					// Player gets first move. dialogue box with options. turn based. 

					//Update screen
					SDL_RenderPresent(gRenderer);
				}





				//if (gameState.textAvailable) {
				//	renderDialogue(gRenderer, gFont);
				//	player.currentState = State::Idle;
				//}
				//else if (gameState.OpenedMenu) {
				//	renderMenuSideBySide(gRenderer, gFont);
				//	player.currentState = State::Idle;

				//}
				//else if (gameState.inMenu) {
				//	//renderMenuSideBySide(gRenderer, gFont);
				//	MS_renderMenu(gRenderer, gFont);
				//	player.currentState = State::Idle;
				//}

				//if (gameState.checkFlag) {
				//	bool someone = false;
				//	for (const auto& entity : Entities) {
				//		if (SDL_HasIntersection(&player.m_CheckBox, &entity->m_Collider)) { // &entity->m_Collider
				//			printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
				//			printf(std::to_string(entity->m_EntityID).c_str());
				//			someone = true;
				//			if (entity->m_NPC) {
				//				entity->m_NPC->m_checked = true;
				//			}
				//		}
				//	}
				//	if (!someone) {
				//		gameState.Text.clear();
				//		gameState.Text.push_back("Who are you talking to..?");
				//		gameState.textAvailable = true;
				//	}
				//	gameState.checkFlag = false;
				//}




				////Update screen
				//SDL_RenderPresent(gRenderer);

				// shitty handmade vsync
				//If frame finished early
				int frameTicks = capTimer.getTicks();
				if (frameTicks < SCREEN_TICKS_PER_FRAME)
				{
					//Wait remaining time
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}

			} // ____END OF MAIN WHILE____

		} // end of if not loaded media.
	} // end of if not sdl2 init

	//Free resources and close SDL
	close();

	return 0;
}
