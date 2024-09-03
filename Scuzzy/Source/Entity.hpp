#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"

class Entity
{
public:
	Entity(Vector2f p_pos, SDL_Rect FrameRect, LTexture* p_tex, int framecount, std::vector<SDL_Rect> Clips);
	Vector2f& getPos()
	{
		return m_Pos;
	}
	float getAngle()
	{
		return angle;
	}
	Vector2f getScale()
	{
		return scale;
	}
	void setPos(float x, float y);
	void setScale(float w, float h);
	void setAngle(float angle);
	LTexture* getTex(); //SDL_Texture* getTex();
	void update(double deltaTime, SDL_Rect CameraRect);
	SDL_Rect getCurrentFrame();

	int currentFrameCount = 0;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)
	int FRAME_COUNT; // number of frames in animation.
	bool moving = false;

private:
	Vector2f m_Pos;
	float angle = 0;
	Vector2f scale = Vector2f(1, 1);
	SDL_Rect currentFrame;
	//SDL_Texture* m_Texture;
	LTexture* m_Texture;
	std::vector<SDL_Rect> m_Clips;
};