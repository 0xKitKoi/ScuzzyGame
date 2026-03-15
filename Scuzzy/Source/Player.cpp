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
#include "Source/GameState.hpp"
#include "Source/Projectile.hpp"
#include "Source/Magic.hpp"


extern int screenwidth;
extern int screenheight;
extern float avgFPS;
extern Camera camera;

extern std::vector<std::shared_ptr<Entity>> Entities;

// const int GRID_CELL_SIZE = 100;
// const int GRID_WIDTH = gameState.levelWidth / GRID_CELL_SIZE;
// const int GRID_HEIGHT = gameState.levelHeight / GRID_CELL_SIZE;
//extern std::vector<SDL_Rect> grid[GRID_WIDTH][GRID_HEIGHT];

extern LTexture gTextTexture;
extern std::vector<SDL_Rect> staticCollisionBoxes; // Map walls and obstacles.

extern void MS_renderMenu(SDL_Renderer* renderer, TTF_Font* font);

extern SDL_Rect CheckBox;

int FRAME_COUNT = 4; // each animation has 4 frames

// Each "State" holds rects that point to different images on the sprite sheet.
std::vector<SDL_Rect> UpWalking; // the first frame of each state is the "idle" frame.
std::vector<SDL_Rect> idleDown; 
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
	m_HeartCollider = { screenwidth /2, screenheight/2, 32, 32 };

	m_VelX = 0;
	m_VelY = 0;
	currentState = State::Idle;
	currentDirection = Direction::Down;
	currentFrame = 0; // animtion frame count.


	/*
	// C++ needs to know: "How big is each element?"
	std::vector<Magic> spells;  // Each element is sizeof(Magic) bytes

	// When you do this:
	spells.push_back(DoubleOrNothing());  // DoubleOrNothing might be bigger!
	// C++ can't resize the already-allocated vector slots, so it slices
	
	if (gameState.hasDoubleOrNothing) {
		m_Abilities.push_back(DoubleOrNothing());
	}
	if (gameState.hasHealing) {
		m_Abilities.push_back(Heal());
	}
	if (gameState.hasScuzzy) {
		m_Abilities.push_back(Scuzzy());
	}
	*/
	if (gameState.hasDoubleOrNothing) {
		m_Abilities.push_back(std::make_unique<DoubleOrNothing>());
	}
	if (gameState.hasHealing) {
		m_Abilities.push_back(std::make_unique<Heal>());
	}
	if (gameState.hasScuzzy) {
		m_Abilities.push_back(std::make_unique<Scuzzy>());
	}


	int pixelsize = 128; // size of each frame in sprite sheet. offset.
	SDL_Rect gSpriteClips[6*6] = {

		// The Player's sprite sheet is a 6x6 grid. Each cell is 128x128 pixels.
		{ 0,			 	0, pixelsize, pixelsize },
		{ pixelsize, 	 	0, pixelsize, pixelsize },
		{ pixelsize * 2, 	0, pixelsize, pixelsize },
		{ pixelsize * 3, 	0, pixelsize, pixelsize },
		{ pixelsize * 4, 	0, pixelsize, pixelsize },
		{ pixelsize * 5, 	0, pixelsize, pixelsize },

		{ 0, 				pixelsize, pixelsize, pixelsize },
		{ pixelsize, 		pixelsize, pixelsize, pixelsize },
		{ pixelsize * 2, 	pixelsize, pixelsize, pixelsize },
		{ pixelsize * 3, 	pixelsize, pixelsize, pixelsize },
		{ pixelsize * 4, 	pixelsize, pixelsize, pixelsize },
		{ pixelsize * 5, 	pixelsize, pixelsize, pixelsize },

		{ 0,				pixelsize * 2,	pixelsize, pixelsize },
		{ pixelsize,		pixelsize * 2,	pixelsize, pixelsize },
		{ pixelsize*2,		pixelsize * 2,	pixelsize, pixelsize },
		{ pixelsize*3,		pixelsize * 2,	pixelsize, pixelsize },
		{ pixelsize*4,		pixelsize * 2,	pixelsize, pixelsize },
		{ pixelsize*5,		pixelsize * 2,	pixelsize, pixelsize },

		{ 0,				pixelsize * 3,	pixelsize, pixelsize },
		{ pixelsize,		pixelsize * 3,	pixelsize, pixelsize },
		{ pixelsize * 2,	pixelsize * 3,	pixelsize, pixelsize },
		{ pixelsize * 3,	pixelsize * 3,	pixelsize, pixelsize },
		{ pixelsize * 4,	pixelsize * 3,	pixelsize, pixelsize },
		{ pixelsize * 5,	pixelsize * 3,	pixelsize, pixelsize },

		{ 0,				pixelsize * 4, 	pixelsize, pixelsize },
		{ pixelsize ,		pixelsize * 4, 	pixelsize, pixelsize },
		{ pixelsize*2 ,		pixelsize * 4, 	pixelsize, pixelsize },
		{ pixelsize*3 ,		pixelsize * 4, 	pixelsize, pixelsize },
		{ pixelsize*4 ,		pixelsize * 4, 	pixelsize, pixelsize },
		{ pixelsize*5 ,		pixelsize * 4, 	pixelsize, pixelsize },

		{ 0, 				pixelsize * 5, 	pixelsize, pixelsize },
		{ pixelsize, 		pixelsize * 5, 	pixelsize, pixelsize },
		{ pixelsize * 2, 	pixelsize * 5, 	pixelsize, pixelsize },
		{ pixelsize * 3, 	pixelsize * 5, 	pixelsize, pixelsize },
		{ pixelsize * 4, 	pixelsize * 5, 	pixelsize, pixelsize },
		{ pixelsize * 5, 	pixelsize * 5, 	pixelsize, pixelsize }


	}; // sprite sheet. dont you dare touch. 
	

	//Load sprite sheet texture
	if (!SpriteSheet.loadFromFile("data/playerspritesheet.png")) //noss cartman 128.png
	{
		printf("Failed to load sprite sheet texture!\n");
	}
	else
	{	
		// SPRITE SHEET MAPPING
		// this showed me true propain.

		UpWalking.push_back(gSpriteClips[0]);
		UpWalking.push_back(gSpriteClips[1]);
		UpWalking.push_back(gSpriteClips[2]);
		UpWalking.push_back(gSpriteClips[3]);
		UpWalking.push_back(gSpriteClips[4]);

		DownWalking.push_back(gSpriteClips[5]);
		DownWalking.push_back(gSpriteClips[6]);
		DownWalking.push_back(gSpriteClips[7]);
		DownWalking.push_back(gSpriteClips[8]);
		DownWalking.push_back(gSpriteClips[9]);

		LeftWalking.push_back(gSpriteClips[10]);
		LeftWalking.push_back(gSpriteClips[11]);
		LeftWalking.push_back(gSpriteClips[12]);
		LeftWalking.push_back(gSpriteClips[13]);
		LeftWalking.push_back(gSpriteClips[14]);

		RightWalking.push_back(gSpriteClips[15]);
		RightWalking.push_back(gSpriteClips[16]);
		RightWalking.push_back(gSpriteClips[17]);
		RightWalking.push_back(gSpriteClips[18]);
		RightWalking.push_back(gSpriteClips[19]);

		// DownWalking.push_back(gSpriteClips[20]); not needed ? 
		Emotes.push_back(gSpriteClips[21]);
		Emotes.push_back(gSpriteClips[22]);
		Emotes.push_back(gSpriteClips[23]);
		Emotes.push_back(gSpriteClips[24]);
		Emotes.push_back(gSpriteClips[25]);
		Emotes.push_back(gSpriteClips[26]);
		Emotes.push_back(gSpriteClips[27]);
		Emotes.push_back(gSpriteClips[28]);
		Emotes.push_back(gSpriteClips[29]);
		Emotes.push_back(gSpriteClips[30]);
		Emotes.push_back(gSpriteClips[31]);
		Emotes.push_back(gSpriteClips[32]);
		Emotes.push_back(gSpriteClips[33]);
	}
}

Player::~Player() {
	// Destructor
	// Free the texture if it was loaded
	if (gameState.DebugMode)
		printf("Player destructor called.\n");
	SpriteSheet.free();
}


/// <summary>
/// Handles Player movement, animations, collision detection.
/// </summary>
/// <param name="boxes">Vector of Rects full of map's boundaries and obstructions.</param>
/// <param name="deltaTime">Scales movement and animations based on time between frames.</param>
void Player::Update(std::vector<SDL_Rect*>& boxes, float deltaTime) {
	if (gameState.HP <= 0) {	return; 	}

	if (gameState.inMenu || gameState.inFight || gameState.wonFight) {
		reset({ float(m_PosX), float(m_PosY) }); // this is a bug fix for movement gliding errors.
		return;
	}


	// check box updating.
	if (currentDirection == Direction::Up) {
		m_CheckBox = { m_PosX + 30, m_PosY , 60,60 };
	}
	else if (currentDirection == Direction::Down) {
		m_CheckBox = { m_PosX+45, m_PosY + 90, 50,50 };
	}
	else if (currentDirection == Direction::Left) {
		m_CheckBox = { m_PosX - 5, m_PosY + 50, 50,50 };
	}
	else if (currentDirection == Direction::Right) {
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

	/*
	for (const auto& entity : AllEntities) {
		if (SDL_HasIntersection(&m_Collider, &entity->m_Collider )) { // &entity->m_Collider
			printf(std::to_string(entity->m_EntityID).c_str());

		}
	}
	*/

	// Check for collisions on X axis
	for (const auto& wall : boxes) { // these are from entities that may or may not be moving. Must be pointers.
		if (SDL_HasIntersection(&m_Collider, wall)) {
			m_PosX = originalX; // Revert position
			break;
		}
	}
	for (const auto& wall : staticCollisionBoxes) {
		if (SDL_HasIntersection(&m_Collider, &wall)) {
			m_PosX = originalX; // Revert position
			break;
		}
	}

	// Move along Y axis
	m_PosY += m_VelY * deltaTime;
	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };
	// Check for collisions on Y axis
	for (const auto& wall : boxes) {
		if (SDL_HasIntersection(&m_Collider, wall)) {
			m_PosY = originalY; // Revert position
			break;
		}
	}
	for (const auto& wall : staticCollisionBoxes) {
		if (SDL_HasIntersection(&m_Collider, &wall)) {
			m_PosY = originalY; // Revert position
			break;
		}
	}


	if (m_PosX < 0) {
		m_PosX = originalX;
	}
	if (m_PosX + SpriteWidth > gameState.levelWidth) {
		m_PosX = originalX;
	}
	if (m_PosY < 0) {
		m_PosY = originalY;
	}
	if (m_PosY + SpriteHeight > gameState.levelHeight) {
		m_PosY = originalY;
	}

	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };

	// Debug.
	//SDL_Color textColor = { 0, 0, 0 };
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "DELTA: %f, FPS: %f, Velocity X: %d, \nVelocity Y: %d\nPOS: (%d,%d)\nCOLLIDER POS: (%d,%d)\nCHECKBOX: (%d,%d)", deltaTime, avgFPS,  m_VelX, m_VelY, m_PosX, m_PosY, m_Collider.x, m_Collider.y, m_CheckBox.x, m_CheckBox.y);
	std::string str = buffer;

	if (!gTextTexture.loadFromRenderedText(str, textColor)) {
		printf("[!] Failed to render text texture! Player::Update()\n");
	}

	camera.centerOn(m_PosX , m_PosY );
}


/// <summary>
/// Handles user input. Currently only handles movement. 
/// </summary>
/// <param name="e">SDL checks for key presses. We check for the buttons.</param>
void Player::handleEvent(SDL_Event& e, float deltaTime) {
	if (gameState.inMenu) {
		clearInputState();
		reset({ float(m_PosX), float(m_PosY) });
		return;
	}

	// Advance animation frames
	//lastFrameTime += deltaTime * 1000.0f;
	//if (lastFrameTime >= frameDuration) {
	//	currentFrame = (currentFrame + 1) % /*FRAME_COUNT*/ 5;
	//	lastFrameTime = 0;
	//}
	// Advance animation frames
	lastFrameTime += deltaTime * 1000.0f;
	if (!gameState.inFight) {
		frameDuration = 100;
	}
	else { frameDuration = 400; }
	while (lastFrameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % 4;
		lastFrameTime -= frameDuration;  // Subtract instead of setting to 0
	}

	if (!gameState.inFight) {


		// so the player can hold a button down and open a menu, making it impossible for this to consume the SDL_KeyUP event.
		// this makes it so that when the player quickly presses directions and opens the menu at the same time, when exiting the menu,
		// the player glides because the m_VelX and m_VelY is non zero.
		// this is my workaround for that horrible bug. 
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			switch (e.key.keysym.sym) { 
			case SDLK_UP:    keyUpPressed = true; break;
			case SDLK_DOWN:  keyDownPressed = true; break;
			case SDLK_LEFT:  keyLeftPressed = true; break;
			case SDLK_RIGHT: keyRightPressed = true; break;
			}
		}

		if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_UP:    keyUpPressed = false; break;
			case SDLK_DOWN:  keyDownPressed = false; break;
			case SDLK_LEFT:  keyLeftPressed = false; break;
			case SDLK_RIGHT: keyRightPressed = false; break;
			}
		}
		m_VelX = 0;
		m_VelY = 0;

		if (keyUpPressed)    m_VelY -= MaxVelocity;
		if (keyDownPressed)  m_VelY += MaxVelocity;
		if (keyLeftPressed)  m_VelX -= MaxVelocity;
		if (keyRightPressed) m_VelX += MaxVelocity;
		


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
			//printf("Not in dodge mechanic, skipping heart controls.\n");

			
			if (gameState.doubleOrNothingActive) {
				m_FightSpriteSheet.renderGlow(m_HeartPos.x, m_HeartPos.y, &m_HeartGlowClips[currentFrame]);
			}
			else {
				m_FightSpriteSheet.render(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[currentFrame]);
			}
			

			//SDL_Color debugColor = { 255, 0, 0, 255 };
			//SDL_Color tmp;
			//SDL_GetRenderDrawColor(gRenderer, &tmp.r, &tmp.g, &tmp.b, &tmp.a);
			//SDL_SetRenderDrawColor(gRenderer, debugColor.r, debugColor.g, debugColor.b, debugColor.a);
			//m_HeartTensionCollider = { int(m_HeartPos.x) - 8, int(m_HeartPos.y) - 8, m_HeartClips[0].w + 11, m_HeartClips[0].h + 13 };
			//for (size_t i = 0; i < gameState.enemy->m_EnemyProjectiles.size(); i++) {
			//	if (SDL_HasIntersection( &gameState.enemy->m_EnemyProjectiles.at(i)->m_Collider, &m_HeartTensionCollider )) {
			//		SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255); // green if colliding
			//		SDL_RenderDrawRect(gRenderer, &m_HeartTensionCollider); // draw heart tension hitbox for debugging
			//	}
			//}
			////SDL_RenderDrawRect(gRenderer, &m_HeartTensionCollider); // draw heart tension hitbox for debugging
			//SDL_SetRenderDrawColor(gRenderer, tmp.r, tmp.g, tmp.b, tmp.a);
			return; 
		}
		SDL_Color debugColor = { 255, 0, 0, 255 };
		SDL_Color tmp;
		SDL_GetRenderDrawColor(gRenderer, &tmp.r, &tmp.g, &tmp.b, &tmp.a);
		SDL_SetRenderDrawColor(gRenderer, debugColor.r, debugColor.g, debugColor.b, debugColor.a);
		m_HeartTensionCollider = { int(m_HeartPos.x) - 8, int(m_HeartPos.y) - 8, m_HeartClips[0].w + 11, m_HeartClips[0].h + 13 };
		// Projectiles do this check with the player's m_HeartCollider. Here, I want to check if they collide with the tension box, and if they do, increase tension and mark the projectile as having hit the tension box so it doesn't increase tension multiple times.
		for (size_t i = 0; i < gameState.enemy->m_EnemyProjectiles.size(); i++) {
			if (SDL_HasIntersection(&gameState.enemy->m_EnemyProjectiles.at(i)->m_Collider, &m_HeartTensionCollider)) {
				SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255); // green if colliding
				SDL_RenderDrawRect(gRenderer, &m_HeartTensionCollider); // draw heart tension hitbox for debugging
				if (gameState.enemy->m_EnemyProjectiles.at(i)->m_TensionHit) {
					continue; // skip if already hit tension box. 
				}
				gameState.TensionMeter += 5; // increase tension meter on hit
				gameState.enemy->m_EnemyProjectiles.at(i)->m_TensionHit = true; // mark projectile as having hit tension box
			}
		}
		//SDL_RenderDrawRect(gRenderer, &m_HeartTensionCollider); // draw heart tension hitbox for debugging
		SDL_SetRenderDrawColor(gRenderer, tmp.r, tmp.g, tmp.b, tmp.a);


		if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
			// Adjust the velocity and update direction/state
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				m_HeartVelocity.y -= MaxVelocity;
				keyUpPressed = true;
				break;
			case SDLK_DOWN:
				m_HeartVelocity.y += MaxVelocity;
				keyDownPressed = true;
				break;
			case SDLK_LEFT:
				m_HeartVelocity.x -= MaxVelocity;
				keyLeftPressed = true;
				break;
			case SDLK_RIGHT:
				m_HeartVelocity.x += MaxVelocity;
				keyRightPressed = true;
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
		

		m_HeartCollider = { int(m_HeartPos.x) +8, int(m_HeartPos.y) +8, 15, 15 };

		//m_FightSpriteSheet.renderGlow(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[currentFrame]);
		if (gameState.doubleOrNothingActive) {
			m_FightSpriteSheet.renderGlow(m_HeartPos.x, m_HeartPos.y, &m_HeartGlowClips[currentFrame]);
		}
		else {
			m_FightSpriteSheet.render(m_HeartPos.x, m_HeartPos.y, &m_HeartClips[currentFrame]);
		}
		SDL_RenderDrawRect(gRenderer, &m_HeartCollider); // draw heart hitbox for debugging
		if (gameState.DebugMode)
			printf("currentFrame: %d\n", currentFrame);
		//FS_HandleInput(gRenderer, gFont, e);

	}
}



void Player::reset(Vector2f initPos) {
	//printf("Player::reset called.\n");
	m_PosX = initPos.x;
	m_PosY = initPos.y;
	m_VelX = 0;
	m_VelY = 0;
	currentState = State::Idle;
	//currentDirection = Direction::Down;
	currentFrame = 0; // animtion frame count.
	clearInputState();
	m_Collider = { m_PosX + 40, m_PosY + 60, 50, 40 };
	m_HeartCollider = { int(m_HeartPos.x) + 16, int(m_HeartPos.y) + 16, 20, 20 };
	gameState.dead = false;

}

void Player::clearInputState() {
	m_VelX = 0;
	m_VelY = 0;
	keyDownPressed = false;
	keyUpPressed = false;
	keyLeftPressed = false;
	keyRightPressed = false;
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


	//SpriteSheet.render(m_PosX - camX /* + MapoffsetX*/, m_PosY - camY /* + MapoffsetY*/, &srcRect);
	SDL_Rect renderQuad = camera.worldToScreen({ (m_PosX), (m_PosY), (SpriteWidth), (SpriteHeight) });
	//SpriteSheet.render(renderQuad.x, renderQuad.y, &srcRect);
	SDL_RenderCopy(gRenderer, SpriteSheet.getTexture(), &srcRect, &renderQuad);
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
