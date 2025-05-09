#pragma once
#include <memory>  // For std::unique_ptr


#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"

#ifndef ENTITY_H
#define ENTITY_H

//#include "Source/Enemy.hpp"

class Enemy;
class NPC;


class Entity
{
public:
	Entity();
	~Entity();

	/// <summary>
	/// Create like so: auto entity = std::make_shared<Entity>(entityPos, entity_cb, entityRect, &entityTex, 2, clips, 44);
	/// </summary>
	/// <param name="p_pos">Vector2f X and Y coordinates. (X,Y)</param>
	/// <param name="collisionBox">SDL_Rect Collision Box</param>
	/// <param name="FrameRect">SDL_Rect Defualt first frame in sprite sheet.</param>
	/// <param name="p_tex">LTexture* Find from global LTexture vector.</param>
	/// <param name="framecount">Number of frames in animation.</param>
	/// <param name="Clips">std::vector<SDL_Rect> sprite sheet clippings.</param>
	/// <param name="EntityID">int, Entity ID number for sprite / Dialougue Connections.</param>
	Entity(Vector2f p_pos, SDL_Rect collisionBox, SDL_Rect FrameRect, std::shared_ptr<LTexture> p_tex, int framecount, std::vector<SDL_Rect> Clips, int EntityID);

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

	std::shared_ptr<LTexture> getTex(); //SDL_Texture* getTex();
	void Update(float deltaTime, SDL_Rect CameraRect, SDL_Rect PlayerPOS);
	
	SDL_Rect getCurrentFrame();
	
	void setEnemy(std::shared_ptr<Enemy> newChild);
	void setNPC(std::shared_ptr<NPC> newchild);


	int currentFrameCount = 0;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)
	int FRAME_COUNT; // number of frames in animation.
	bool moving = false;

	SDL_Rect m_SpriteRect = {0,0,0,0};
	//Vector2f m_Pos; // did not play nice. ints for now.
	int m_PosX, m_PosY;
	SDL_Rect m_Collider; // Collision Box
	SDL_Rect m_FOV; // entities field of vision.

	int m_EntityID = 0;

	std::shared_ptr<Enemy> m_Enemy;
	std::shared_ptr<NPC> m_NPC;

private:
	float angle = 0;
	Vector2f scale = Vector2f(1, 1);
	SDL_Rect currentFrame;
	//SDL_Texture* m_Texture;
	//LTexture m_Texture; // Sprite sheet.
	std::shared_ptr<LTexture> m_Texture;
	std::vector<SDL_Rect> m_Clips;
	


};



#endif // ENTITY_H