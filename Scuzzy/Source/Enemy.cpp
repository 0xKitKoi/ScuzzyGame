#include "Source/Enemy.hpp"
#include "Source/Entity.hpp"
#include "Source/Math.hpp"
#include <stdio.h>
#include "Source/GameState.hpp"
#include "Source/FightSystem.hpp"
#include <random>


extern float lerp(float x, float y, float t);
extern std::shared_ptr<LTexture> getTexture(const std::string& filename);   

//Enemy::Enemy(Entity& p ) : m_Entity(p) {}
//Enemy::Enemy(std::shared_ptr<Entity> entity) : m_Entity(entity) {}

int randomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}



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


//void Enemy::Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) {
void Enemy::Update(float deltaT, Camera CameraRect, SDL_Rect PlayerPos) {

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
                std::random_device rd;
                std::mt19937 gen(rd());
                

                // init the projectiles
                for (int i = 0; i < m_projectileCount; i++) {
			        //m_EnemyProjectiles.push_back(std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1));
                    // using the m_EnemyProjectile as a template, create new projectiles
                    float subx = float(randomInt(0, gameState.screenwidth));
                    float suby = float(randomInt(0, gameState.screenheight));
                    m_EnemyProjectiles.push_back(std::make_shared<Projectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f( subx, suby ), Vector2f(200,200), 1));
                    // randomize vector2f(x,y) position:
		        }
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

        /*
        // Advance animation frames
	lastFrameTime += deltaTime * 1000.0f;
	while (lastFrameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % 4;
		lastFrameTime -= frameDuration;  // Subtract instead of setting to 0
	}
        */
        lastFrameTime += deltaT * 1000.0f;
        while (lastFrameTime >= frameDuration) {
            currentFrameCount = (currentFrameCount + 1) % /*this->m_Entity->FRAME_COUNT*/ 4;
            //lastFrameTime = 0;
            lastFrameTime -= frameDuration;
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
        for (int i = 0; i < m_projectileCount; i++) {
            if (m_EnemyProjectiles[i]) {

                m_EnemyProjectiles[i]->Update(deltaT, gameState.player->m_HeartPos);
                // Check for collision with player or going off-screen
                // (Collision detection code would go here)
                if (!m_EnemyProjectiles[i]->m_Active) {
                    continue; // skip inactive projectiles
                }
                m_EnemyProjectiles[i]->m_SpriteSheet->render(int(m_EnemyProjectiles[i]->m_Position.x), int(m_EnemyProjectiles[i]->m_Position.y) /*, &m_EnemyProjectile->m_SpriteClip*/ ); 
            }
        }   
    }
}

/// @brief This returns text based off of an action chosen by the player in a fight. This should be overwritten with custom behavior for each special enemy.
/// @param actionIndex Index of enemy->m_actions the action chosen by the player in a fight.
/// @return A string to tell the player their actions have done something special.
std::string Enemy::FightActionResponse(int actionIndex) {
    printf("[!] BASE Enemy::FightActionResponse called with actionIndex: %d\n", actionIndex);
    printf("[!] I don't have any custom behavior! returning default response.\n");
	// by default, return the action response at the given index.
    if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
        return "[!] Invalid action. actionIndex out of range. How did you even do this?";
    }
    return m_ActionResponse[actionIndex];
}


void Enemy::ResetProjectiles() {
    //m_EnemyProjectiles.clear();
        // init the projectiles
        for (int i = 0; i < m_projectileCount; i++) {
		    //m_EnemyProjectiles.push_back(std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1));
            // using the m_EnemyProjectile as a template, create new projectiles
            float subx = float(randomInt(0, gameState.screenwidth));
            float suby = float(randomInt(0, gameState.screenheight));
            //m_EnemyProjectiles.push_back(std::make_shared<m_EnemyProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f( subx, suby ), Vector2f(200,200), 1));
            m_EnemyProjectiles.at(i)->m_Position = Vector2f( subx, suby );
            m_EnemyProjectiles.at(i)->m_Init = true;
            m_EnemyProjectiles.at(i)->m_Active = true;
            // randomize vector2f(x,y) position:
        }
        float subx = float(randomInt(0, gameState.screenwidth));
        float suby = float(randomInt(0, gameState.screenheight));
        m_EnemyProjectiles[0] = std::make_shared<HomingProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f(subx, suby), Vector2f(200, 200), 1);
}


void Enemy::move(Vector2f targetPos) {
	//Vector2f prev = m_Entity.getPos();

}

Enemy::~Enemy() {
    // cleanup if needed
    m_ActionResponse.clear();
    m_Actions.clear();
    m_EnemyDialogue.clear();
    m_EnemySpriteClips.clear();
    m_EnemyProjectiles.clear();

}
