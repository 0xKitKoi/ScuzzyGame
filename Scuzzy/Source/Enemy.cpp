#include "Source/Enemy.hpp"
#include "Source/Entity.hpp"
#include <stdio.h>


extern float lerp(float x, float y, float t);

Enemy::Enemy(Entity& p) : m_Entity(p) {}

void Enemy::Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) {
	// move to the player if player is in POV box. once touch player, set game state to fight mode.
	if (SDL_HasIntersection(&m_Entity.m_Collider, &CameraRect)) {
		if (SDL_HasIntersection(&m_Entity.m_FOV, &PlayerPos)) {
			printf("Enemy can see the player!\n");
		}
	}
}

void Enemy::move(Vector2f targetPos) {
	//Vector2f prev = m_Entity.getPos();

}
