#include "Source/Enemy.hpp"
#include "Source/Entity.hpp"
#include "Source/Math.hpp"
#include <stdio.h>
#include "Source/GameState.hpp"
#include "Source/FightSystem.hpp"


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
        //gameState.inFight = true;
        //gameState.enemyID = m_EnemyID;
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

    /// TODO fix enemy collisions !!!!!!!
    if (!alive) { return; }
	//if (gameState.inFight) { return; }
	if (gameState.FightStarted) { return; }

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

            if (SDL_HasIntersection(&m_Entity->m_Collider, &PlayerPos)) {
				printf("Enemy has reached the player! Starting fight...\n");
                gameState.enemyID = m_EnemyID;
                gameState.enemy = this;
                gameState.FightStarted = true;
                gameState.player->m_HeartPos = { float(gameState.screenwidth) / 2.0f - 32.0f, float(gameState.screenheight) / 2.0f - 32.0f };
                gameState.Plot = 0;
				FS_InitFight();
                this->alive = false;
            }
	}
    else {
		m_Entity->moving = false;
	}
}


/// <summary>
/// IN FIGHT UPDATE TODO: give position to render at
/// </summary>
/// <param name="deltaT">float Delta time</param>
/// <param name="screenheight">int</param>
/// <param name="screenwidth">int</param>
void Enemy::Update(float deltaT, int screenheight, int screenwidth ) {
	if (gameState.fightState == FightState::ENEMY_DIALOGUE) {
		//SDL_Rect enemysprite = this->m_EnemySpriteClips[0];
        
        //this->m_Entity->getCurrentFrame();


        lastFrameTime += deltaT * 1000.0f;
        if (lastFrameTime >= frameDuration) {
            currentFrameCount = (currentFrameCount + 1) % this->m_Entity->FRAME_COUNT;
            lastFrameTime = 0;
        }

        //enemysprite = m_EnemySpriteClips[currentFrameCount]; // render the sprite at index of animation
        //this->m_Entity->getTex()->render((screenwidth / 2) - 128, (screenheight / 2) - 128 * 2, &this->m_Entity->getCurrentFrame());
        this->m_EnemyFightSpriteSheet->render((screenwidth / 2) - 128, (screenheight / 2) - 128 * 2, &this->m_EnemySpriteClips[currentFrameCount]);

	}
    else {
		//this->m_Entity->getTex()->render((screenwidth / 2) - 128, (screenheight / 2) - 128 * 2, &this->m_Entity->getCurrentFrame());
        this->m_EnemyFightSpriteSheet->render((screenwidth / 2) - 128, (screenheight / 2) - 128 * 2, &this->m_EnemySpriteClips[currentFrameCount]);
    }
    //if (gameState.fightState == FightState::ENEMY_TURN) {
    //    //
    //    gameState.enemy->HP -= gameState.enemy->m_AttackDamage; // Example of attacking the player
    //    if (gameState.enemy->HP <= 0) {
    //        gameState.wonFight = true; // Player wins
    //        gameState.inFight = false; // End fight
    //    }
    //}
    if (gameState.fightState == FightState::DODGE_MECHANIC) {
        // Update enemy projectile(s) ..?
        if (m_EnemyProjectile) {
            m_EnemyProjectile->Update(deltaT, gameState.fightPlayerPos);
            // Check for collision with player or going off-screen
            // (Collision detection code would go here)
        }
	}
}

void Enemy::move(Vector2f targetPos) {
	//Vector2f prev = m_Entity.getPos();

}
