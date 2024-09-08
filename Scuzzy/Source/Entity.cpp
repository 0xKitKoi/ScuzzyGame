#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/LTexture.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

/*
Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, int framecount)
	:m_Pos(p_pos), m_Texture(p_tex)
*/

Entity::Entity() : m_Enemy(nullptr) {}
//m_Pos(p_pos)
Entity::Entity(Vector2f p_pos, SDL_Rect FrameRect, LTexture* p_tex, int framecount, std::vector<SDL_Rect> Clips)
	:m_Texture(p_tex), currentFrame(FrameRect), m_Enemy(nullptr)
{
	//currentFrame.x = 0;
	//currentFrame.y = 0;
	//currentFrame.w = 128;
	//currentFrame.h = 128;
	//SDL_QueryTexture(p_tex, NULL, NULL, &currentFrame.w, &currentFrame.h);

	m_PosX = p_pos.x;
	m_PosY = p_pos.y;


	FRAME_COUNT = framecount;
	m_Clips = Clips;
	m_Collider = FrameRect; // might fucky wucky later LOL
	// Idea behind this is to make a bigger box around the entity.
	// This will hopefully let the entity detect the player around it.
	m_FOV = { -m_Collider.x * 4, -m_Collider.y * 4, m_Collider.x * 4, m_Collider.y * 4 };

}

Entity::~Entity() {
	// The unique_ptr will automatically delete the child if it exists
}

LTexture* Entity::getTex() //SDL_Texture* Entity::getTex
{
	return m_Texture;
}

void Entity::Update(float deltaTime, SDL_Rect CameraRect, SDL_Rect PlayerPos)
{
	SDL_Rect srcRect;

	if (m_Enemy) {
		m_Enemy->Update(deltaTime, CameraRect, PlayerPos);
	}

	if (moving) {
		// Calculates index of frame to use in animation.
		lastFrameTime += deltaTime * 1000.0f;
		if (lastFrameTime >= frameDuration) {
			currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
			lastFrameTime = 0;
		}
	}
	srcRect = m_Clips[currentFrameCount]; // render the sprite at index of animation

	m_Collider = { m_PosX, m_PosY, m_SpriteRect.w, m_SpriteRect.h };
	SDL_RenderDrawRect(gRenderer, &m_Collider);
	// SpriteSheet.render(m_PosX - camX, m_PosY - camY, &srcRect);
	m_Texture->render(m_PosX - CameraRect.x, m_PosY - CameraRect.y, &srcRect);
}


void Entity::setAngle(float p_angle)
{
	angle = p_angle;
}

void Entity::setScale(float w, float h)
{
	scale.x = w;
	scale.y = h;
}

SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}


void Entity::setEnemy(std::shared_ptr<Enemy> newChild) {
	//m_Enemy = std::move(newChild);
	m_Enemy = newChild;
}