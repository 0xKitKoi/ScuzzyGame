#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include <memory>  // For std::unique_ptr


#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"
//#include "Source/Enemy.hpp"

class Enemy;


class Entity
{
public:
	Entity();
	~Entity();

	Entity(Vector2f p_pos, SDL_Rect collisionBox, SDL_Rect FrameRect, LTexture* p_tex, int framecount, std::vector<SDL_Rect> Clips);

	float getAngle()
	{
		return angle;
	}
	Vector2f getScale()
	{
		return scale;
	}

	void setScale(float w, float h);
	void setAngle(float angle);

	LTexture* getTex(); //SDL_Texture* getTex();
	void Update(float deltaTime, SDL_Rect CameraRect, SDL_Rect PlayerPOS);
	
	SDL_Rect getCurrentFrame();
	
	void setEnemy(std::shared_ptr<Enemy> newChild);


	int currentFrameCount = 0;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)
	int FRAME_COUNT; // number of frames in animation.
	bool moving = false;

	SDL_Rect m_SpriteRect;
	//Vector2f m_Pos; // did not play nice. ints for now.
	int m_PosX, m_PosY;
	SDL_Rect m_Collider; // Collision Box
	SDL_Rect m_FOV; // entities field of vision.
private:
	float angle = 0;
	Vector2f scale = Vector2f(1, 1);
	SDL_Rect currentFrame;
	//SDL_Texture* m_Texture;
	LTexture* m_Texture; // Sprite sheet.
	std::vector<SDL_Rect> m_Clips;
	std::shared_ptr<Enemy> m_Enemy;


};



#endif // ENTITY_H