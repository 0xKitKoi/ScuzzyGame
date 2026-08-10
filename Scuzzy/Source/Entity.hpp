#pragma once
#include <memory>  // For std::unique_ptr


#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"
#include <unordered_map>
#include <string>

#ifndef ENTITY_H
#define ENTITY_H

//#include "Source/Enemy.hpp"

class Enemy;
class NPC;

	struct Animation {
		std::vector<SDL_Rect> frames;
		float frameDuration = 100.0f; // ms per frame, independent per-animation
		bool loop = true;
	};


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
	//void Update(float deltaTime, SDL_Rect CameraRect, SDL_Rect PlayerPOS);
	void Update(float deltaTime, Camera CameraRect, SDL_Rect PlayerPOS);
	
	SDL_Rect getCurrentFrame();
	
	void setEnemy(std::shared_ptr<Enemy> newChild);
	void setNPC(std::shared_ptr<NPC> newchild);

	void EnableBackLayer(SDL_Rect* backClips, int frameCountMax, float frameDuration);

	// CutScene related functions
    	Vector2f m_targetPosition{ 0.0f, 0.0f };
    	bool m_isLerping = false;
		float m_MoveSpeed;
    void MoveTo(Vector2f target, float speed = 300.0f) {
        m_targetPosition = target;
        m_MoveSpeed  = speed;
        m_isLerping   = true;
    }



	int currentFrameCount = 0;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)
	int FRAME_COUNT; // number of frames in animation.
	bool moving = false;
	bool m_AnimationFinished = false;
	bool m_PlayAnimationOnce = false;

	SDL_Rect m_SpriteRect = {0,0,0,0};
	//Vector2f m_Pos; // did not play nice. ints for now.
	int m_PosX, m_PosY;
	SDL_Rect m_Collider; // Collision Box
	SDL_Rect m_FOV; // entities field of vision.

	int m_EntityID = 0;

	std::shared_ptr<Enemy> m_Enemy;
	std::shared_ptr<NPC> m_NPC;
	std::vector<SDL_Rect> m_Clips;

	std::shared_ptr<LTexture> m_Texture;

	    // Optional second animation layer, rendered behind the main sprite.
    // Inactive unless EnableBackLayer() has been called for this entity.
    bool m_HasBackLayer = false;
    SDL_Rect* m_BackClips = nullptr;
    int m_BackFrameCountMax = 0;
    int m_BackFrameCount = 0;
    float m_BackFrameTime = 0.0f;
    float m_BackFrameDuration = 0.0f;


		// new members, alongside your existing m_Clips / FRAME_COUNT / etc.
	std::unordered_map<std::string, Animation> m_Animations;
	std::string m_CurrentAnimation;
	int   m_AnimFrameIndex   = 0;
	float m_AnimFrameTimer   = 0.0f;
	bool  m_AnimPlaying      = true;
	bool  m_AnimFinishedFlag = false;

	void AddAnimation(const std::string& name, std::vector<SDL_Rect> frames, float frameDuration, bool loop = true);
	void PlayAnimation(const std::string& name, bool restart = true);
	void StopAnimation();
	void ResumeAnimation();
	void SetAnimationSpeed(float ms);
	bool IsAnimationFinished() const { return m_AnimFinishedFlag; }
	const std::string& GetCurrentAnimation() const { return m_CurrentAnimation; }

private:
	float angle = 0;
	Vector2f scale = Vector2f(1, 1);
	SDL_Rect currentFrame;
	//SDL_Texture* m_Texture;
	//LTexture m_Texture; // Sprite sheet.
	
};



#endif // ENTITY_H
