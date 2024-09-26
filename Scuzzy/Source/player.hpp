#pragma once
#include "Source/LTexture.hpp"
#include <SDL.h>
#include "Source/math.hpp"
#include <vector>


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
	State currentState;
	Direction currentDirection;
	int currentFrame;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)


	static const int SpriteHeight = 128;
	static const int SpriteWidth = 128;

	static const int MaxVelocity = 300;
	//static const int MaxAcceleration = 300;

	Player(Vector2f initPos); // init
	//void Update(const std::vector<SDL_Rect>& walls, float deltaTime);
	void Update(std::vector<SDL_Rect*>& walls, float deltaTime);

	void handleEvent(SDL_Event& e);
	//void move(const std::vector<SDL_Rect>& walls, float deltaTime);
	//void move(std::vector<SDL_Rect> grid[40][40]);
	void render(int camX, int camY);
	int GetPosX();
	int GetPosY();
	SDL_Rect GetCollider();
	SDL_Rect& GetColliderAddress();

private:
	int m_PosX, m_PosY;
	int m_VelX, m_VelY;
	SDL_Rect m_Collider;
	LTexture SpriteSheet;
	LTexture CurrentSprite;

	bool keyUpPressed = false;
	bool keyDownPressed = false;
	bool keyLeftPressed = false;
	bool keyRightPressed = false;
};