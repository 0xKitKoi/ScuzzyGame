#include "Source/Player.hpp"
#include "Source/LTexture.hpp"
#include "Source/Math.hpp"
#include "Source/GameState.hpp"
extern GameState gameState;
#include <vector>
#include <stdio.h>
#include <Source/Entity.hpp>
#include <Source/NPC.hpp>

#include "Source/FightSystem.hpp"

//bool checkCollision(SDL_Rect a, SDL_Rect b);
//const int SCREEN_WIDTH = 1920;
//const int SCREEN_HEIGHT = 1080;
//const int LEVEL_WIDTH = 4000;
//const int LEVEL_HEIGHT = 4000;
//extern int levelWidth;
//extern int levelHeight;
//extern int MapoffsetX;
//extern int MapoffsetY;
extern int screenwidth;
extern int screenheight;

const int GRID_CELL_SIZE = 100;
const int GRID_WIDTH = gameState.levelWidth / GRID_CELL_SIZE;
const int GRID_HEIGHT = gameState.levelHeight / GRID_CELL_SIZE;
//extern std::vector<SDL_Rect> grid[GRID_WIDTH][GRID_HEIGHT];
extern LTexture gTextTexture;

//extern std::vector<std::shared_ptr<Entity>> Entities;


extern void MS_renderMenu(SDL_Renderer* renderer, TTF_Font* font);

extern SDL_Rect CheckBox;

int FRAME_COUNT = 4; // each animation has 4 frames

// Each "State" holds rects that point to different images on the sprite sheet.
std::vector<SDL_Rect> UpWalking; // the first frame of each state is the "idle" frame.
std::vector<SDL_Rect> idleDown; // TODO: fix these horrible names.
std::vector<SDL_Rect> DownWalking;
std::vector<SDL_Rect> LeftWalking;
std::vector<SDL_Rect> RightWalking;
std::vector<SDL_Rect> Emotes;

/// <summary>
/// Initializes Player.
/// </summary>
/// <param name="initPos">Starting position. Should be loaded from save file.</param>
Player::Player(Vector2f initPos, std::vector<std::shared_ptr<Entity>>& entityVec) : AllEntities(entityVec) {
	m_PosX = initPos.x;
	m_PosY = initPos.y;
	m_Collider.w = SpriteWidth;
	m_Collider.h = SpriteHeight;

	m_CheckBox.x = m_PosX;
	m_CheckBox.y = m_PosY;
	m_CheckBox.h = SpriteHeight;
	m_CheckBox.w = SpriteWidth;


	m_VelX = 0;
	m_VelY = 0;
	currentState = State::Idle;
	currentDirection = Direction::Down;
	currentFrame = 0; // animtion frame count.

	SDL_Rect gSpriteClips[6*6]; // sprite sheet. dont you dare touch. 
	int pixelsize = 128; // size of each frame in sprite sheet. offset.

	//Load sprite sheet texture
	if (!SpriteSheet.loadFromFile("data/playerspritesheet.png")) //noss cartman 128.png
	{
		printf("Failed to load sprite sheet texture!\n");
	}
	else
	{
		// SPRITE SHEET MAPPING
		// this showed me true propain.

		// Looking up not moving [0]
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = pixelsize;
		gSpriteClips[0].h = pixelsize;
		UpWalking.push_back(gSpriteClips[0]);

		// Looking up moving 1 [1]
		gSpriteClips[1].x = pixelsize;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = pixelsize;
		gSpriteClips[1].h = pixelsize;
		UpWalking.push_back(gSpriteClips[1]);

		// Looking up moving 2 [2]
		gSpriteClips[2].x = pixelsize * 2;
		gSpriteClips[2].y = 0;
		gSpriteClips[2].w = pixelsize;
		gSpriteClips[2].h = pixelsize;
		UpWalking.push_back(gSpriteClips[2]);

		// Looking up moving 3 [3]
		gSpriteClips[3].x = pixelsize * 3;
		gSpriteClips[3].y = 0;
		gSpriteClips[3].w = pixelsize;
		gSpriteClips[3].h = pixelsize;
		UpWalking.push_back(gSpriteClips[3]);

		// Looking up moving 4 [4]
		gSpriteClips[4].x = pixelsize * 4;
		gSpriteClips[4].y = 0;
		gSpriteClips[4].w = pixelsize;
		gSpriteClips[4].h = pixelsize;
		UpWalking.push_back(gSpriteClips[4]);

		// Looking down not moving [5]
		gSpriteClips[5].x = pixelsize * 5;
		gSpriteClips[5].y = 0;
		gSpriteClips[5].w = pixelsize;
		gSpriteClips[5].h = pixelsize;
		DownWalking.push_back(gSpriteClips[5]);

		// Looking down moving 1 [6]
		gSpriteClips[6].x = 0;
		gSpriteClips[6].y = pixelsize;
		gSpriteClips[6].w = pixelsize;
		gSpriteClips[6].h = pixelsize;
		DownWalking.push_back(gSpriteClips[6]);

		// Looking down moving 2 [7]
		gSpriteClips[7].x = pixelsize;
		gSpriteClips[7].y = pixelsize;
		gSpriteClips[7].w = pixelsize;
		gSpriteClips[7].h = pixelsize;
		DownWalking.push_back(gSpriteClips[7]);

		// Looking down moving 3 [8]
		gSpriteClips[8].x = pixelsize * 2;
		gSpriteClips[8].y = pixelsize;
		gSpriteClips[8].w = pixelsize;
		gSpriteClips[8].h = pixelsize;
		DownWalking.push_back(gSpriteClips[8]);

		// Looking down moving 4 [9]
		gSpriteClips[9].x = pixelsize * 3;
		gSpriteClips[9].y = pixelsize;
		gSpriteClips[9].w = pixelsize;
		gSpriteClips[9].h = pixelsize;
		DownWalking.push_back(gSpriteClips[9]);

		// Looking Left not moving [10]
		gSpriteClips[10].x = pixelsize * 4;
		gSpriteClips[10].y = pixelsize;
		gSpriteClips[10].w = pixelsize;
		gSpriteClips[10].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[10]);

		// Looking Left moving 1 [11]
		gSpriteClips[11].x = pixelsize * 5;
		gSpriteClips[11].y = pixelsize;
		gSpriteClips[11].w = pixelsize;
		gSpriteClips[11].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[11]);

		// Looking Left moving 2 [12]
		gSpriteClips[12].x = 0;
		gSpriteClips[12].y = pixelsize * 2;
		gSpriteClips[12].w = pixelsize;
		gSpriteClips[12].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[12]);

		// Looking Left moving 3 [13]
		gSpriteClips[13].x = pixelsize;
		gSpriteClips[13].y = pixelsize * 2;
		gSpriteClips[13].w = pixelsize;
		gSpriteClips[13].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[13]);

		// Looking Left moving 4 [14]
		gSpriteClips[14].x = pixelsize * 2;
		gSpriteClips[14].y = pixelsize * 2;
		gSpriteClips[14].w = pixelsize;
		gSpriteClips[14].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[14]);

		// Looking Right not moving [15]
		gSpriteClips[15].x = pixelsize * 3;
		gSpriteClips[15].y = pixelsize * 2;
		gSpriteClips[15].w = pixelsize;
		gSpriteClips[15].h = pixelsize;
		RightWalking.push_back(gSpriteClips[15]);

		// Looking Right moving 1 [16]
		gSpriteClips[16].x = pixelsize * 4;
		gSpriteClips[16].y = pixelsize * 2;
		gSpriteClips[16].w = pixelsize;
		gSpriteClips[16].h = pixelsize;
		RightWalking.push_back(gSpriteClips[16]);

		// Looking Right moving 2 [17]
		gSpriteClips[17].x = pixelsize * 5;
		gSpriteClips[17].y = pixelsize * 2;
		gSpriteClips[17].w = pixelsize;
		gSpriteClips[17].h = pixelsize;
		RightWalking.push_back(gSpriteClips[17]);

		// Looking Right moving 3 [18]
		gSpriteClips[18].x = 0;
		gSpriteClips[18].y = pixelsize * 3;
		gSpriteClips[18].w = pixelsize;
		gSpriteClips[18].h = pixelsize;
		RightWalking.push_back(gSpriteClips[18]);

		// Looking Right moving 4 [19]
		gSpriteClips[19].x = pixelsize;
		gSpriteClips[19].y = pixelsize * 3;
		gSpriteClips[19].w = pixelsize;
		gSpriteClips[19].h = pixelsize;
		RightWalking.push_back(gSpriteClips[19]);

		// Looking Down not moving  [20]
		gSpriteClips[20].x = pixelsize * 2;
		gSpriteClips[20].y = pixelsize * 3;
		gSpriteClips[20].w = pixelsize;
		gSpriteClips[20].h = pixelsize;
		DownWalking.push_back(gSpriteClips[20]);

		// Looking Down suprised  [21]
		gSpriteClips[21].x = pixelsize * 3;
		gSpriteClips[21].y = pixelsize * 3;
		gSpriteClips[21].w = pixelsize;
		gSpriteClips[21].h = pixelsize;
		Emotes.push_back(gSpriteClips[21]);

		// Looking Right Suprised  [22]
		gSpriteClips[22].x = pixelsize * 4;
		gSpriteClips[22].y = pixelsize * 3;
		gSpriteClips[22].w = pixelsize;
		gSpriteClips[22].h = pixelsize;
		Emotes.push_back(gSpriteClips[22]);

		// Looking Left Suprised  [23]
		gSpriteClips[23].x = pixelsize * 5;
		gSpriteClips[23].y = pixelsize * 3;
		gSpriteClips[23].w = pixelsize;
		gSpriteClips[23].h = pixelsize;
		Emotes.push_back(gSpriteClips[23]);

		// Looking Up Suprised  [24]
		gSpriteClips[24].x = 0;
		gSpriteClips[24].y = pixelsize * 4;
		gSpriteClips[24].w = pixelsize;
		gSpriteClips[24].h = pixelsize;
		Emotes.push_back(gSpriteClips[24]);

		// Idle 1  [25]
		gSpriteClips[25].x = pixelsize;
		gSpriteClips[25].y = pixelsize * 4;
		gSpriteClips[25].w = pixelsize;
		gSpriteClips[25].h = pixelsize;
		Emotes.push_back(gSpriteClips[25]);

		// Idle 2 [26]
		gSpriteClips[26].x = pixelsize * 2;
		gSpriteClips[26].y = pixelsize * 4;
		gSpriteClips[26].w = pixelsize;
		gSpriteClips[26].h = pixelsize;
		Emotes.push_back(gSpriteClips[26]);

		// Idle 3  [27]
		gSpriteClips[27].x = pixelsize * 3;
		gSpriteClips[27].y = pixelsize * 4;
		gSpriteClips[27].w = pixelsize;
		gSpriteClips[27].h = pixelsize;
		Emotes.push_back(gSpriteClips[27]);

		// Looking Down Confused  [28]
		gSpriteClips[28].x = pixelsize * 4;
		gSpriteClips[28].y = pixelsize * 4;
		gSpriteClips[28].w = pixelsize;
		gSpriteClips[28].h = pixelsize;
		Emotes.push_back(gSpriteClips[28]);

		// Looking Down goober  [29]
		gSpriteClips[29].x = pixelsize * 5;
		gSpriteClips[29].y = pixelsize * 4;
		gSpriteClips[29].w = pixelsize;
		gSpriteClips[29].h = pixelsize;
		Emotes.push_back(gSpriteClips[29]);

		// Looking down Pissed  [30]
		gSpriteClips[30].x = 0;
		gSpriteClips[30].y = pixelsize * 5;
		gSpriteClips[30].w = pixelsize;
		gSpriteClips[30].h = pixelsize;
		Emotes.push_back(gSpriteClips[30]);

		// POG Left  [31]
		gSpriteClips[31].x = pixelsize;
		gSpriteClips[31].y = pixelsize * 5;
		gSpriteClips[31].w = pixelsize;
		gSpriteClips[31].h = pixelsize;
		Emotes.push_back(gSpriteClips[31]);

		// POG Right  [32]
		gSpriteClips[32].x = pixelsize * 2;
		gSpriteClips[32].y = pixelsize * 5;
		gSpriteClips[32].w = pixelsize;
		gSpriteClips[32].h = pixelsize;
		Emotes.push_back(gSpriteClips[32]);

		// DED  [33]
		gSpriteClips[33].x = pixelsize * 4;
		gSpriteClips[33].y = pixelsize * 5;
		gSpriteClips[33].w = pixelsize;
		gSpriteClips[33].h = pixelsize;
		Emotes.push_back(gSpriteClips[33]);
	}
}

Player::~Player() {
	// Destructor
	// Free the texture if it was loaded
	printf("Player destructor called.\n");
	SpriteSheet.free();
}


/// <summary>
/// Handles Player movement, animations, collision detection.
/// </summary>
/// <param name="walls">Vector of Rects full of map's boundaries and obstructions.</param>
/// <param name="deltaTime">Scales movement and animations based on time between frames.</param>
//void Player::Update(const std::vector<SDL_Rect>& walls, float deltaTime) {
void Player::Update(std::vector<SDL_Rect*>& boxes, float deltaTime) {
	//if (gameState.inMenu) {
	//	m_VelX = 0;
	//	m_VelY = 0;
	//	currentState = State::Idle;
	//	return;
	//}
	if (gameState.HP <= 0) {
		// player has died, run death screen and load from save.
		/*gameState.Text.clear();
		gameState.Text.push_back("You greened out lil bro, nap time for you.");
		gameState.textAvailable = true;*/
		return;
	}

	if (gameState.inMenu || gameState.inFight || gameState.wonFight) {
		reset({ float(m_PosX), float(m_PosY) });
		//m_VelX = 0;
		//m_VelY = 0;
		//currentState = State::Idle;
		//keyDownPressed = false;
		//keyUpPressed = false;
		//keyLeftPressed = false;
		//keyRightPressed = false;
		gameState.wonFight = false;
		return;
	}


	// check box updating.
	if (currentDirection == Direction::Up) {
		//m_CheckBox.x = m_PosX;
		//m_CheckBox.y = m_PosY - 30;
		m_CheckBox = { m_PosX + 30, m_PosY , 60,60 };
	}
	else if (currentDirection == Direction::Down) {
		//m_CheckBox.x = m_PosX;
		//m_CheckBox.y = m_PosY + 30;
		m_CheckBox = { m_PosX+45, m_PosY + 90, 50,50 };
	}
	else if (currentDirection == Direction::Left) {
		//m_CheckBox.x = m_PosX - 30;
		//m_CheckBox.y = m_PosY;
		m_CheckBox = { m_PosX - 5, m_PosY + 50, 50,50 };
	}
	else if (currentDirection == Direction::Right) {
		//m_CheckBox.x = m_PosX + 30;
		//m_CheckBox.y = m_PosY;
		m_CheckBox = { m_PosX + 90, m_PosY +50, 50,50 };
	}

	

	// Clamp Velocity.
	if (m_VelX > MaxVelocity) {
		m_VelX = MaxVelocity;
	}
	if (m_VelX < -MaxVelocity) {
		m_VelX = -MaxVelocity;
	}
	if (m_VelY > MaxVelocity) {
		m_VelY = MaxVelocity;
	}
	if (m_VelY < -MaxVelocity) {
		m_VelY = -MaxVelocity;
	}


	// Store the initial position
	int originalX = m_PosX;
	int originalY = m_PosY;

	// Move along X axis
	m_PosX += m_VelX * deltaTime;
	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 }; // Scuffed fatass's collision box




	for (const auto& entity : AllEntities) {
		if (SDL_HasIntersection(&m_Collider, &entity->m_Collider )) { // &entity->m_Collider
			printf(std::to_string(entity->m_EntityID).c_str());

		}
	}

	// Check for collisions on X axis
	for (const auto& wall : boxes) {
		if (SDL_HasIntersection(&m_Collider, wall)) {
			m_PosX = originalX; // Revert position
			
			break;
		}
	}

	// Move along Y axis
	m_PosY += m_VelY * deltaTime;
	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };
	//m_Collider.x = m_PosX + 40; // Update collider position
	//m_Collider.y = m_PosY + 60; // Update collider position
	// Check for collisions on Y axis
	for (const auto& wall : boxes) {
		if (SDL_HasIntersection(&m_Collider, wall)) {
			m_PosY = originalY; // Revert position
			
			break;
		}
	}

	// Boundary checks (keep player within level bounds)
	// Account for map offsets in the boundary checks
	int effectiveLeftBound = 0 + gameState.MapoffsetX;
	int effectiveTopBound = 0 + gameState.MapoffsetY;
	int effectiveRightBound = gameState.levelWidth + gameState.MapoffsetX ;
	int effectiveBottomBound = gameState.levelHeight + gameState.MapoffsetY;

	/*
	// Boundary checks - keep player within actual map bounds
	if (m_PosX < 0 || m_PosX + SpriteWidth > gameState.levelWidth) {
    	m_PosX = originalX;
	}
	if (m_PosY < 0 || m_PosY + SpriteHeight > gameState.levelHeight) {
    	m_PosY = originalY;
	}
	*/

	
	// Check left/right boundaries
	if (m_PosX < 0 || m_PosX < effectiveLeftBound || m_PosX + SpriteWidth > effectiveRightBound) {
		m_PosX = originalX; // Revert position if out of bounds
	}

	// Check top/bottom boundaries
	if (m_PosY < 0 || m_PosY < effectiveTopBound || m_PosY + SpriteHeight > effectiveBottomBound) {
		m_PosY = originalY; // Revert position if out of bounds
	}
	


	//// Boundary checks (keep player within level bounds)
	//if (m_PosX < 0 || m_PosX + SpriteWidth > levelWidth) { // LEVEL_WIDTH
	//	m_PosX = originalX; // Revert position if out of bounds
	//}

	//if (m_PosY < 0 || m_PosY + SpriteHeight > levelHeight) { // LEVEL_HEIGHT
	//	m_PosY = originalY; // Revert position if out of bounds
	//}
	////m_PosX = m_PosX + MapoffsetX; // TODO: add this to boundary checks.
	////m_PosY = m_PosY + MapoffsetY;

	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };

	// Debug.
	SDL_Color textColor = { 0, 0, 0 };
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "DELTA: %f\n Velocity X: %d, \nVelocity Y: %d\nPOS: (%d,%d)\nCOLLIDER POS: (%d,%d)\nCHECKBOX: (%d,%d)", deltaTime, m_VelX, m_VelY, m_PosX, m_PosY, m_Collider.x, m_Collider.y, m_CheckBox.x, m_CheckBox.y);
	std::string str = buffer;

	if (!gTextTexture.loadFromRenderedText(str, textColor)) {
		printf("Failed to render text texture!\n");
	}
}


/// <summary>
/// Handles user input. Currently only handles movement. 
/// </summary>
/// <param name="e">SDL checks for key presses. We check for the buttons.</param>
void Player::handleEvent(SDL_Event& e, float deltaTime) {
	if (gameState.inMenu || gameState.FightStarted) {
		reset({ float(m_PosX), float(m_PosY) });
		//m_VelX = 0;
		//m_VelY = 0;
		//currentState = State::Idle;
		//keyDownPressed = false;
		//keyUpPressed = false;
		//keyLeftPressed = false;
		//keyRightPressed = false;
		
	}

	// Advance animation frames
	lastFrameTime += deltaTime * 1000.0f;
	if (lastFrameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % FRAME_COUNT;
		lastFrameTime = 0;
	}

	if (!gameState.inFight) {
		//if (gameState.inMenu) {
		//	reset({ float(m_PosX), float(m_PosY) });
		//	//m_VelX = 0;
		//	//m_VelY = 0;
		//	//currentState = State::Idle;
		//	//keyDownPressed = false;
		//	//keyUpPressed = false;
		//	//keyLeftPressed = false;
		//	//keyRightPressed = false;
		//	return;
		//}



		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			// Adjust the velocity and update direction/state
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				m_VelY -= MaxVelocity;
				keyUpPressed = true;
				currentState = State::Walking;
				currentDirection = Direction::Up;
				break;
			case SDLK_DOWN:
				m_VelY += MaxVelocity;
				keyDownPressed = true;
				currentState = State::Walking;
				currentDirection = Direction::Down;
				break;
			case SDLK_LEFT:
				m_VelX -= MaxVelocity;
				keyLeftPressed = true;
				currentState = State::Walking;
				currentDirection = Direction::Left;
				break;
			case SDLK_RIGHT:
				m_VelX += MaxVelocity;
				keyRightPressed = true;
				currentState = State::Walking;
				currentDirection = Direction::Right;
				break;
			}
		}

		// If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
			// Adjust the velocity and update key states
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				m_VelY += MaxVelocity;
				keyUpPressed = false;
				break;
			case SDLK_DOWN:
				m_VelY -= MaxVelocity;
				keyDownPressed = false;
				break;
			case SDLK_LEFT:
				m_VelX += MaxVelocity;
				keyLeftPressed = false;
				break;
			case SDLK_RIGHT:
				m_VelX -= MaxVelocity;
				keyRightPressed = false;
				break;
			}
		}

		// Determine the current state based on which keys are still pressed
		if (keyUpPressed || keyDownPressed || keyLeftPressed || keyRightPressed) {
			currentState = State::Walking;

			// Update direction based on the most recent active key
			if (keyUpPressed) {
				currentDirection = Direction::Up;
			}
			else if (keyDownPressed) {
				currentDirection = Direction::Down;
			}
			else if (keyLeftPressed) {
				currentDirection = Direction::Left;
			}
			else if (keyRightPressed) {
				currentDirection = Direction::Right;
			}
		}
		else {
			currentState = State::Idle;
		}
	}
	else { // IN FIGHT MODE
		if (gameState.fightState != FightState::DODGE_MECHANIC) { 
			m_FightSpriteSheet.render(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[currentFrame]);
			return; 
		}

		//m_VelX = 0;
		//m_VelY = 0;
		//currentState = State::Idle;
		//keyDownPressed = false;
		//keyUpPressed = false;
		//keyLeftPressed = false;
		//keyRightPressed = false;
		//reset({ float(m_PosX), float(m_PosY) });
		

		// IN FIGHT MOVEMENT AND SPRITE STATE AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		/*
			Here's where we decide what this game is about.
			I could add more state vectors containing SDL_Rects pointing to sprites on another sprite sheet. 
			I want to do an undertale and earthbound inspired fight mechanic. 
			I did enjoy undertale's dodging mechanics but I also feel like the classic text based combat is a good start for my first game.
			The concept art shows my ideas of clashing these together. I dont know If I want that rn.

			I've decided to focus on NPC dialogue first. 
		
		*/

		//FS_HandleInput(gRenderer, gFont, e);


		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			// Adjust the velocity and update direction/state
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				m_HeartVelocity.y -= MaxVelocity;
				keyUpPressed = true;
				//currentState = State::Walking;
				//currentDirection = Direction::Up;
				break;
			case SDLK_DOWN:
				m_HeartVelocity.y += MaxVelocity;
				keyDownPressed = true;
				//currentState = State::Walking;
				//currentDirection = Direction::Down;
				break;
			case SDLK_LEFT:
				m_HeartVelocity.x -= MaxVelocity;
				keyLeftPressed = true;
				//currentState = State::Walking;
				//currentDirection = Direction::Left;
				break;
			case SDLK_RIGHT:
				m_HeartVelocity.x += MaxVelocity;
				keyRightPressed = true;
				//currentState = State::Walking;
				//currentDirection = Direction::Right;
				break;
			}
		}

		// If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
			// Adjust the velocity and update key states
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				m_HeartVelocity.y += MaxVelocity;
				keyUpPressed = false;
				break;
			case SDLK_DOWN:
				m_HeartVelocity.y -= MaxVelocity;
				keyDownPressed = false;
				break;
			case SDLK_LEFT:
				m_HeartVelocity.x += MaxVelocity;
				keyLeftPressed = false;
				break;
			case SDLK_RIGHT:
				m_HeartVelocity.x -= MaxVelocity;
				keyRightPressed = false;
				break;
			}
		}
		// Clamp Velocity.
		if (m_HeartVelocity.x > MaxVelocity) {
			m_HeartVelocity.x = MaxVelocity;
		}
		if (m_HeartVelocity.x < -MaxVelocity) {
			m_HeartVelocity.x = -MaxVelocity;
		}
		if (m_HeartVelocity.y > MaxVelocity) {
			m_HeartVelocity.y = MaxVelocity;
		}
		if (m_HeartVelocity.y < -MaxVelocity) {
			m_HeartVelocity.y = -MaxVelocity;
		}
		m_HeartPos.x += m_HeartVelocity.x * deltaTime;
		m_HeartPos.y += m_HeartVelocity.y * deltaTime;
		// print heat pos, velocity and deltatime
		printf("Heart pos: (%f, %f) | Velocity: (%d, %d) | DeltaTime: %f", m_HeartPos.x, m_HeartPos.y, m_VelX, m_VelY, deltaTime );


		//printf("Heart Pos: (%f,%f)\n", m_HeartPos.x, m_HeartPos.y);
		//printf("Velocity: (%d,%d)\n", m_VelX, m_VelY);
		if (currentFrame % 2 == 0)
			m_FightSpriteSheet.render(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[0]);
		else
			m_FightSpriteSheet.render(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[1]);

		//FS_HandleInput(gRenderer, gFont, e);

	}
}



void Player::reset(Vector2f initPos) {
	m_PosX = initPos.x;
	m_PosY = initPos.y;
	currentState = State::Idle;
	//currentDirection = Direction::Down;
	currentFrame = 0; // animtion frame count.
	m_VelX = 0;
	m_VelY = 0;
	keyDownPressed = false;
	keyUpPressed = false;
	keyLeftPressed = false;
	keyRightPressed = false;
	//m_HeartPos = { float(m_PosX), float(m_PosY) };
	//m_HeartPos = { float(screenwidth) / 2.0f - 32.0f, float(screenheight) / 2.0f - 32.0f };
	//m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };
	//m_CheckBox = { m_PosX + 30, m_PosY , 60,60 };


}


/// <summary>
/// Render's the player. Also handles State of animation like direction. 
/// </summary>
/// <param name="camX">Camera Position.</param>
/// <param name="camY">Camera Position.</param>
void Player::render(int camX, int camY) {
	// Advance animation frames
	lastFrameTime += gameState.deltaTime * 1000.0f;
	if (lastFrameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % FRAME_COUNT;
		lastFrameTime = 0;
	}


	SDL_Rect srcRect;
	if (currentState == State::Idle && currentDirection == Direction::Down) {
		srcRect = DownWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Up) {
		srcRect = UpWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Left) {
		srcRect = LeftWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Right) {
		srcRect = RightWalking[0];
	}

	if (currentState == State::Walking && currentDirection == Direction::Down) {
		if (currentFrame > DownWalking.size()) {
			//srcRect = idleDown[idleDown.size()];
			srcRect = DownWalking[DownWalking.size()];
		}
		else {
			srcRect = DownWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Right) {
		if (currentFrame > RightWalking.size()) {
			srcRect = RightWalking[RightWalking.size()];
		}
		else {
			srcRect = RightWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Left) {
		if (currentFrame > LeftWalking.size()) {
			srcRect = LeftWalking[LeftWalking.size()];
		}
		else {
			srcRect = LeftWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Up) {
		if (currentFrame > UpWalking.size()) {
			srcRect = UpWalking[UpWalking.size()];
		}
		else {
			srcRect = UpWalking[currentFrame];
		}
		
	}


	// hey dickhead tell the Ltexture to render with the rect you have!!!
	//SpriteSheet.render(m_PosX, m_PosY, &srcRect);
	SpriteSheet.render(m_PosX - camX /* + MapoffsetX*/, m_PosY - camY /* + MapoffsetY*/, &srcRect);

	//m_Collider.x = m_Collider.x - camX + MapoffsetX;
	//m_Collider.y = m_Collider.y - camY + MapoffsetY;
}

int Player::GetPosX() {
	return m_PosX;
}

int Player::GetPosY() {
	return m_PosY;
}

void Player::SetPosX(int X) {
	m_PosX = X;
	return;
}

void Player::SetPosY(int Y) {
	m_PosY = Y;
	return; 
}

SDL_Rect Player::GetCollider() {
	return m_Collider;
}

SDL_Rect& Player::GetColliderAddress() {
	return m_Collider;
}
