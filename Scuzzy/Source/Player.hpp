#pragma once
#include "Source/LTexture.hpp"
#include <SDL.h>
#include "Source/Math.hpp"
#include <vector>
#include <Source/Entity.hpp>
#include "Source/Magic.hpp"


enum class State {
	Idle,
	Walking,
	Running,
	// Add more states as needed
};

enum class Direction {
	Up,
	Down,
	Left,
	Right,
	// Add more directions if needed
};

class Player {
public:


	// Each "State" holds rects that point to different images on the sprite sheet.
	std::vector<SDL_Rect> UpWalking; // the first frame of each state is the "idle" frame.
	std::vector<SDL_Rect> idleDown;
	std::vector<SDL_Rect> DownWalking;
	std::vector<SDL_Rect> LeftWalking;
	std::vector<SDL_Rect> RightWalking;
	std::vector<SDL_Rect> Emotes;

	Player(Vector2f initPos, std::vector<std::shared_ptr<Entity>>& entityVec); // init
	~Player();
	void reset(Vector2f initPos);
	void clearInputState();
	//void Update(const std::vector<SDL_Rect>& walls, float deltaTime);
	void Update(std::vector<SDL_Rect*>& walls, float deltaTime);

	void handleEvent(SDL_Event& e, float deltaTime);
	//void move(const std::vector<SDL_Rect>& walls, float deltaTime);
	//void move(std::vector<SDL_Rect> grid[40][40]);
	void render(int camX, int camY);

	void RenderSoul(SDL_Renderer* renderer);

	int GetPosX();
	int GetPosY();
	void SetPosX(int X);
	void SetPosY(int Y);
	SDL_Rect GetCollider();
	SDL_Rect& GetColliderAddress();


	State currentState;
	Direction currentDirection;
	int currentFrame;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 500; // 100 // Time each frame is displayed (milliseconds)


	static const int SpriteHeight = 128;
	static const int SpriteWidth = 128;

	static const int MaxVelocity = 300;
	//static const int MaxAcceleration = 300;

	std::vector<std::shared_ptr<Entity>>& AllEntities;
	SDL_Rect m_CheckBox;

	bool m_Invisible = false;
	LTexture m_FightSpriteSheet;
	Vector2f m_HeartPos; // = { float(screenWidth) / 2.0f - 32.0f, float(screenheight) / 2.0f - 32.0f };
	Vector2f m_HeartVelocity;
	SDL_Rect m_HeartClips[4] = { SDL_Rect{0,0,32,32}, SDL_Rect{32,0,32,32}, SDL_Rect{32 * 2, 0, 32, 32}, SDL_Rect{0,32,32,32} };
	SDL_Rect m_HeartGlowClips[4] = { SDL_Rect{32,32,32,32}, SDL_Rect{32*2,32,32,32}, SDL_Rect{0, 32*2, 32, 32}, SDL_Rect{32,32*2,32,32} };
	LTexture CurrentSprite; // need to access for fading.
	SDL_Rect m_HeartCollider;
	SDL_Rect m_HeartTensionCollider;

	//std::vector<Magic> m_Abilities;
	//std::vector<Magic*> m_Abilities;
	std::vector<std::unique_ptr<Magic>> m_Abilities;

	int m_PosX, m_PosY;
	int m_VelX, m_VelY;
	SDL_Rect m_Collider;

	LTexture SpriteSheet;
	SDL_Rect m_CutsceneClip = { 0, 0, SpriteWidth, SpriteHeight }; // Default cutscene clip
private:
	
	
	//LTexture CurrentSprite;
	
	

	bool menuOpened = false;

	bool keyUpPressed = false;
	bool keyDownPressed = false;
	bool keyLeftPressed = false;
	bool keyRightPressed = false;
};
