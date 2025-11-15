#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Source/Entity.hpp"
#include "Source/Math.hpp"

class Projectile {
public:
	// we need projectiles. each projectile has a position, speed, direction, sprite, and damage value.
	// they also need to be updated each frame, and should have a custom update function depending on type of projectile.
	// I want collisions to be handled by the player, since near misses will be added to a tension meter.
	bool m_Active = false;
	LTexture* m_SpriteSheet;
	SDL_Rect m_SpriteClip;
	Vector2f m_Position;
	Vector2f m_TargetPosition;
	Vector2f m_Velocity;
	int m_Damage;

	Projectile(LTexture* SpriteSheet, SDL_Rect SpriteClip, Vector2f Position, Vector2f Velocity, int Damage) : m_SpriteSheet(SpriteSheet), m_SpriteClip(SpriteClip),
		m_Position(Position), m_Velocity(Velocity), m_Damage(Damage) {
	}
	virtual void Update(float deltaT, Vector2f PlayerPos);
	
};
#endif