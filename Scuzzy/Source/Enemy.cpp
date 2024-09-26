#include "Source/Enemy.hpp"
#include "Source/Entity.hpp"
#include "Source/Math.hpp"
#include <stdio.h>
#include "Source/GameState.hpp"


extern float lerp(float x, float y, float t);


//Enemy::Enemy(Entity& p ) : m_Entity(p) {}
//Enemy::Enemy(std::shared_ptr<Entity> entity) : m_Entity(entity) {}


float duration = 2.0f;
float elapsed_time = 0.0f;

float speed = 350.0f;

Vector2f Enemy::moveEntity(Vector2f pos, float deltaTime, Vector2f target) {
    // Calculate the direction vector (dx, dy)
    float dx = target.x - pos.x;
    float dy = target.y - pos.y;

    // Calculate the distance to the target
    float distance = sqrt(dx * dx + dy * dy);

    // If the entity is close enough to the target, stop moving
    if (distance < 0.1f) {
        printf("Too close");
        gameState.inFight = true;
        gameState.enemyID = m_EnemyID;
        return pos;
    }

    // Normalize the direction vector (dx, dy)
    dx /= distance;
    dy /= distance;

    // Move the entity in the direction of the target based on speed and deltaTime
    pos.x += dx * speed * deltaTime;
    pos.y += dy * speed * deltaTime;

    // Optional: Print current position for debugging
    //std::cout << "Entity Position: (" << pos.x << ", " << pos.y << ")\n";
    return pos;
}


void Enemy::Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) {
	// move to the player if player is in POV box. once touch player, set game state to fight mode.
	if (SDL_HasIntersection(&m_Entity->m_FOV, &PlayerPos)) {
			//printf("Enemy can see the player!\n");
			m_Entity->moving = true;
			//m_Entity.m_PosX += lerp(m_Entity.m_PosX, PlayerPos.x, deltaT);
			//m_Entity.m_PosY += lerp(m_Entity.m_PosY, PlayerPos.y, deltaT);
            Vector2f in = { (float)m_Entity->m_PosX, (float)m_Entity->m_PosY };
            Vector2f out = moveEntity(in, deltaT, {(float)PlayerPos.x, (float)PlayerPos.y});
            m_Entity->m_PosX = out.x;
            m_Entity->m_PosY = out.y;
	}
    else {
		m_Entity->moving = false;
	}
}

void Enemy::move(Vector2f targetPos) {
	//Vector2f prev = m_Entity.getPos();

}
