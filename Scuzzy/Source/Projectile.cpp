
#include "Source/GameState.hpp"
#include "Source/Projectile.hpp"
#include <SDL_mixer.h>

extern float lerp(float x, float y, float t);

extern Mix_Chunk* gPlayerHurtSound; // player gets hurt

//Projectile::Projectile(LTexture* SpriteSheet, SDL_Rect SpriteClip, Vector2f Position, Vector2f Velocity, int Damage) : m_SpriteSheet(SpriteSheet), m_SpriteClip(SpriteClip),
//m_Position(Position), m_Velocity(Velocity), m_Damage(Damage) {}

void Projectile::Update(float deltaT, Vector2f PlayerPos) { // DEFAULT UPDATE FUNCTION[ LERP TO PLAYER POSITION]
	// using playerpos, get a target position (that the player WAS in when this is called) and lerp towards it. player should be able to dodge this.
	// when this is called the first time, store the target position into m_TargetPosition and lerp towards that.
	///printf("BASE CLASS Projectile Update Called\n");
	if (!m_Active) {
		return; // do nothing if not active
	}

	if (m_Init) {
		// this is the first update call, set target position.
		m_TargetPosition.x = PlayerPos.x;
		m_TargetPosition.y = PlayerPos.y;

		Vector2f launchDirection = m_TargetPosition - m_Position;
		float launchDistance = launchDirection.Length();
		if (launchDistance > 0.0001f) {
			launchDirection = launchDirection / launchDistance;
		}
		else {
			launchDirection = { 1.0f, 0.0f };
		}

		float launchSpeed = m_Velocity.Length();
		if (launchSpeed <= 0.0f) {
			launchSpeed = 200.0f;
		}

		m_Velocity = launchDirection * launchSpeed;
		m_Init = false;

	}

	// Move in the launch direction until the projectile leaves the screen.
	m_Position.x += m_Velocity.x * deltaT;
	m_Position.y += m_Velocity.y * deltaT;
	m_Collider = SDL_Rect{ int(m_Position.x), int(m_Position.y), m_SpriteClip.w, m_SpriteClip.h };

	if (m_Position.x + m_SpriteClip.w < -64 ||
		m_Position.y + m_SpriteClip.h < -64 ||
		m_Position.x > gameState.screenwidth + 64 ||
		m_Position.y > gameState.screenheight + 64) {
		m_Active = false;
		return;
	}

	if (gameState.DebugMode) {
		printf("BASE CLASS Projectile Update Called\n");
		printf("Projectile Position: (%f, %f)\n", m_Position.x, m_Position.y);
		printf("Target Position: (%f, %f)\n", m_TargetPosition.x, m_TargetPosition.y);
	}

		if (SDL_HasIntersection(&m_Collider, &gameState.player->m_HeartCollider)) {
			printf("Projectile hit the player!\n");
			// deal damage to player
			//gameState.HP -= m_Damage;
			if (gameState.doubleOrNothingActive) {
				gameState.HP = 0;
			}
			gameState.DamageTaken += m_Damage;
			Mix_PlayChannel(-1, gPlayerHurtSound, 0); // play hit sound on impact

			m_Active = false; // deactivate on hit
			m_TensionHit = true;
			//gameState.TensionMeter -= 5;
			gameState.TensionMeter += 5; // cant decide if projectiles should increase or decrease tension on hit
		}

}


