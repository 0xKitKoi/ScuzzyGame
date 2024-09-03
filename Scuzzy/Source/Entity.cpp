#include "Source/Entity.hpp"
#include "Source/LTexture.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

/*
Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, int framecount)
	:m_Pos(p_pos), m_Texture(p_tex)
*/
Entity::Entity(Vector2f p_pos, SDL_Rect FrameRect, LTexture* p_tex, int framecount, std::vector<SDL_Rect> Clips )
	:m_Pos(p_pos), m_Texture(p_tex), currentFrame(FrameRect)
{
	//currentFrame.x = 0;
	//currentFrame.y = 0;
	//currentFrame.w = 128;
	//currentFrame.h = 128;
	//SDL_QueryTexture(p_tex, NULL, NULL, &currentFrame.w, &currentFrame.h);
	FRAME_COUNT = framecount;
	m_Clips = Clips;

}

LTexture* Entity::getTex() //SDL_Texture* Entity::getTex
{
	return m_Texture;
}

void Entity::update(double deltaTime, SDL_Rect CameraRect)
{
	SDL_Rect srcRect;

	if (moving) {
		// Calculates index of frame to use in animation.
		lastFrameTime += deltaTime * 1000.0f;
		if (lastFrameTime >= frameDuration) {
			currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
			lastFrameTime = 0;
		}
	}
	srcRect = m_Clips[currentFrameCount]; // render the sprite at index of animation


	// SpriteSheet.render(m_PosX - camX, m_PosY - camY, &srcRect);
	m_Texture->render(m_Pos.x - CameraRect.x, m_Pos.y - CameraRect.y, &srcRect);
}

void Entity::setPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
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