#pragma once
#include <SDL.h>
#include <math.hpp>
#include "LTexture.hpp"

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

	static const int MaxVelocity = 10;

	Player(Vector2f initPos); // init
	void Update(Uint32 deltaTime);

	void handleEvent(SDL_Event& e);
	void move(SDL_Rect& wall);
	void render();

private:
	int m_PosX, m_PosY;
	int m_VelX, m_VelY;
	SDL_Rect m_Collider;
	LTexture SpriteSheet;
	LTexture CurrentSprite;
};