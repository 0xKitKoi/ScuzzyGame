
#include "Source/GameState.hpp"
#include "Source/Projectile.hpp"

extern float lerp(float x, float y, float t);

//Projectile::Projectile(LTexture* SpriteSheet, SDL_Rect SpriteClip, Vector2f Position, Vector2f Velocity, int Damage) : m_SpriteSheet(SpriteSheet), m_SpriteClip(SpriteClip),
//m_Position(Position), m_Velocity(Velocity), m_Damage(Damage) {}

void Projectile::Update(float deltaT, Vector2f PlayerPos) { // DEFAULT UPDATE FUNCTION[ LERP TO PLAYER POSITION]
	// using playerpos, get a target position (that the player WAS in when this is called) and lerp towards it. player should be able to dodge this.
	// when this is called the first time, store the target position into m_TargetPosition and lerp towards that.
	printf("BASE CLASS Projectile Update Called\n");
	if (!m_Active) {
		return; // do nothing if not active
	}

	if (m_Init) {
		// this is the first update call, set target position.
		m_TargetPosition.x = PlayerPos.x;
		m_TargetPosition.y = PlayerPos.y;
		m_Init = false;
	}
	else {
		// check if we are close enough to target position to stop.
		float dx = m_TargetPosition.x - m_Position.x;
		float dy = m_TargetPosition.y - m_Position.y;
		float distance = sqrt(dx * dx + dy * dy);
		// move towards target position
		 // Normalize the direction vector (dx, dy)
		dx /= distance;
		dy /= distance;

		// Move the entity in the direction of the target based on speed and deltaTime
		m_Position.x += dx * m_Velocity.x * deltaT;
		m_Position.y += dy * m_Velocity.y * deltaT;
		printf("Projectile Position: (%f, %f)\n", m_Position.x, m_Position.y);
		printf("Target Position: (%f, %f)\n", m_TargetPosition.x, m_TargetPosition.y);

		if (SDL_HasIntersection(&m_Collider, &gameState.player->m_HeartCollider)) {
			// deal damage to player
			gameState.HP -= m_Damage;
			m_Active = false; // deactivate on hit
		}

	}
	
	//m_Position.x += m_Velocity.x * deltaT;
	//m_Position.y += m_Velocity.y * deltaT;

}


