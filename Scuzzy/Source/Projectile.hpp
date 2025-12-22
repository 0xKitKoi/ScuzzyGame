#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Source/Entity.hpp"
#include "Source/Math.hpp"

extern float lerp(float x, float y, float t);

class Projectile {
public:
	// we need projectiles. each projectile has a position, speed, direction, sprite, and damage value.
	// they also need to be updated each frame, and should have a custom update function depending on type of projectile.
	// I want collisions to be handled by the player, since near misses will be added to a tension meter.
	bool m_Active = false;
	//LTexture* m_SpriteSheet;
	std::shared_ptr<LTexture> m_SpriteSheet;
	SDL_Rect m_SpriteClip;
	Vector2f m_Position;
	Vector2f m_TargetPosition;
	Vector2f m_Velocity;
	int m_Damage;

	/// @brief Constructor
	/// @param SpriteSheet png 
	/// @param SpriteClip define the size of the sprite
	/// @param Position where do u want it gamer
	/// @param Velocity how fast
	/// @param Damage how hard
	Projectile(/*LTexture* */ std::shared_ptr<LTexture> SpriteSheet, SDL_Rect SpriteClip, Vector2f Position, Vector2f Velocity, int Damage) : m_SpriteSheet(SpriteSheet), m_SpriteClip(SpriteClip),
		m_Position(Position), m_Velocity(Velocity), m_Damage(Damage) {
	}
	virtual void Update(float deltaT, Vector2f PlayerPos);
	
};


class HomingProjectile : public Projectile {
public:
    float m_TurnSpeed = 5.0f;

    using Projectile::Projectile;

    void Update(float deltaT, Vector2f playerPos) override {
        Vector2f direction = (playerPos - m_Position).Normalized();
        m_Velocity = lerp(m_Velocity, direction * m_Velocity.Length(), m_TurnSpeed * deltaT);
        m_Position += m_Velocity * deltaT;
    }
};

class FallingProjectile : public Projectile {
public:
	float m_Gravity = 9.8f;
	using Projectile::Projectile;
	void Update(float deltaT, Vector2f playerPos) override {
		m_Velocity.y += m_Gravity * deltaT;
		m_Position += m_Velocity * deltaT;
		// lerp x position towards player x pos
		m_Position.x = lerp(m_Position.x, playerPos.x, 0.1f * deltaT); // gottem
	}
};


#endif