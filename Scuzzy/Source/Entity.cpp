#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/NPC.hpp"
#include "Source/LTexture.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

/*
Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, int framecount)
	:m_Pos(p_pos), m_Texture(p_tex)
*/

extern bool freezeOverworldActors;


Entity::Entity() :m_Enemy(nullptr), m_NPC(nullptr), FRAME_COUNT(0), currentFrame({ 0,0,0,0 }), m_Collider({ 0,0,0,0 }), m_FOV({ 0,0,0,0 }), m_PosX(0), m_PosY(0) {}
//m_Pos(p_pos)


/// <summary>
/// Creates an entity. Needs a position, a collision box, A Rect for the idle frame, a texture, the number of frames in the aniimation, and a sprite sheet mapping. You can attach a child class to this like Enemy or NPC (Comming soon)
/// </summary>
/// <param name="p_pos">Struct of X and Y coordinates.</param>
/// <param name="CollisionBox">SDL_Rect of the entity's Collision box.</param>
/// <param name="FrameRect">Rect used for indexing sprite sheet for initial frame of animation. TODO: this is not really needed.</param>
/// <param name="p_tex">Sprite Sheet texture. LTexture Object Abstracting away SDL2's raw texture handling. Entity::Update calls LTexture's render().</param>
/// <param name="framecount">Number of frames in the animation.</param>
/// <param name="Clips">Vector of Rects. This is a mapping of the sprite sheet.</param>
Entity::Entity(Vector2f p_pos, SDL_Rect CollisionBox, SDL_Rect FrameRect, std::shared_ptr<LTexture> p_tex, int framecount, std::vector<SDL_Rect> Clips, int EntityID)
	: m_Texture(p_tex), currentFrame(FrameRect), m_Enemy(nullptr), m_NPC(nullptr),  m_Clips(Clips), m_Collider(CollisionBox), m_EntityID(EntityID)
{
	//currentFrame.x = 0;
	//currentFrame.y = 0;
	//currentFrame.w = 128;
	//currentFrame.h = 128; 
	//SDL_QueryTexture(p_tex, NULL, NULL, &currentFrame.w, &currentFrame.h);

	m_PosX = p_pos.x;
	m_PosY = p_pos.y;


	FRAME_COUNT = framecount;
	//m_Clips = Clips;
	//m_Collider = CollisionBox; // { int(p_pos.x),int(p_pos.y),FrameRect.w, FrameRect.h}; // might fucky wucky later LOL
	// Idea behind this is to make a bigger box around the entity.
	// This will hopefully let the entity detect the player around it.
	//m_FOV = { -m_Collider.x * 2, -m_Collider.y * 2, m_Collider.x * 2, m_Collider.y * 2 };
	m_FOV = { (int)((p_pos.x + CollisionBox.w / 2) - (128*3)/2), (int)(p_pos.y + CollisionBox.h / 2) - (128 * 3) / 2, (128 * 3), (128 * 3)};
	//m_Texture = LTexture(p_tex, FrameRect.w, FrameRect.h);
}

Entity::~Entity() {
	// The unique_ptr will automatically delete the child if it exists
	// clear vectors:
	m_Clips.clear();
	m_Texture = nullptr;
}

std::shared_ptr<LTexture> Entity::getTex() //SDL_Texture* Entity::getTex
{
	return m_Texture;
}


/// <summary>
/// Handles Entity Rendering, animations, and . This will also call a child class's update().
/// </summary>
/// <param name="deltaTime">Scales movement and animations with time.</param>
/// <param name="CameraRect">Renders entity based on camera's perspective.</param>
/// <param name="PlayerPos">Player's position for collision detection + enemy chasing</param>
//void Entity::Update(float deltaTime, SDL_Rect CameraRect, SDL_Rect PlayerPos)
// void Entity::Update(float deltaTime, Camera CameraRect, SDL_Rect PlayerPos)
// {
// 	SDL_Rect srcRect;
// 	if (!freezeOverworldActors) {

// 		if (m_isLerping) { // used for CutScenes. this should not interfere with Enemy/NPC.Update() calls.
// 			float dx = m_targetPosition.x - m_PosX;
// 			float dy = m_targetPosition.y - m_PosY;
// 			float dist = sqrtf(dx*dx + dy*dy);
// 			float step = m_MoveSpeed * deltaTime;

// 			if (dist <= step) {
// 				// Close enough — snap and stop
// 				m_PosX = m_targetPosition.x;
// 				m_PosY = m_targetPosition.y;
// 				m_isLerping = false;
// 				moving = false; // this is for animation frame stopping. 
// 			} else {
// 				// Normalize direction, advance by step
// 				m_PosX += (dx / dist) * step;
// 				m_PosY += (dy / dist) * step;
// 			}
// 		}



// 		if (m_Enemy) {
// 			SDL_Rect bruh = PlayerPos; // need to target center of player
// 			bruh.x = bruh.x + bruh.w / 2;
// 			bruh.y = bruh.y + bruh.h / 2;
// 			bruh.w = bruh.w / 2;
// 			bruh.w = bruh.h / 2;
// 			m_Enemy->Update(deltaTime, CameraRect, bruh);
// 		}
// 		if (m_NPC) {
// 			m_NPC->Update(deltaTime, CameraRect, PlayerPos);
// 		}

// 		if (moving && !m_AnimationFinished) {
// 			lastFrameTime += deltaTime * 1000.0f;
// 			if (lastFrameTime >= frameDuration) {
// 				currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
// 				lastFrameTime = 0;
// 				if (m_PlayAnimationOnce && currentFrameCount == 0) {
// 					m_AnimationFinished = true;
// 				}
// 			}

// 			if (m_HasBackLayer) {
// 				m_BackFrameTime += deltaTime * 1000.0f;
// 				if (m_BackFrameTime >= m_BackFrameDuration) {
// 					m_BackFrameCount = (m_BackFrameCount + 1) % m_BackFrameCountMax;
// 					m_BackFrameTime = 0;
// 				}
// 			}
// 		}
// 	}
// 	srcRect = m_Clips[currentFrameCount]; // render the sprite at index of animation

// 	m_Collider = { m_PosX, m_PosY, 128, 128 }; 
// 	m_FOV = { (m_PosX + (m_Collider.w / 2)) - ((128*3)/2) , (m_PosY + (m_Collider.h / 2)) - ((128 * 3) / 2), (int)(currentFrame.w * 3), (int)(currentFrame.h * 3)};

// 	//SDL_RenderDrawRect(gRenderer, &m_Collider);
// 	// SpriteSheet.render(m_PosX - camX, m_PosY - camY, &srcRect);
// 	//m_Texture->render(m_PosX - CameraRect.x, m_PosY - CameraRect.y, &srcRect);
// //	float scale = gameState.mapScaling;
// /////	SDL_Rect enemyScreen = CameraRect.worldToScreen({ m_PosX, m_PosY, srcRect.w, srcRect.h }); // scale should be 1 here because the camera's worldToScreen function already accounts for scaling.
// //	printf("Entity render: world=(%.1f,%.1f) screen=(%d,%d) scale=%.1f\n", 
// //    m_PosX, m_PosY, enemyScreen.x, enemyScreen.y, scale);
//     //SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyScreen);
// /////	m_Texture->render(enemyScreen.x, enemyScreen.y, &srcRect);
// 	int screenX = (m_PosX - CameraRect.x);
// 	int screenY = (m_PosY - CameraRect.y);

// 	//RenderTrail(screenX, screenY);      // draws behind, no-op for normal entities
// 	//m_Texture->render(screenX, screenY, &srcRect);

// 	if (m_HasBackLayer) {
// 		SDL_Rect backSrcRect = m_BackClips[m_BackFrameCount];
// 		SDL_SetTextureAlphaMod(m_Texture->getTexture(), 130); // tune to taste
// 		m_Texture->render(screenX, screenY, &backSrcRect);
// 		SDL_SetTextureAlphaMod(m_Texture->getTexture(), 255); // restore before front layer
// 	}

// 	SDL_Rect renderQuad = { screenX, screenY, m_Collider.w, m_Collider.h };
// 	//SDL_RenderCopy(gRenderer, m_Texture->getTexture(), &srcRect, &renderQuad);
// 	m_Texture->render(screenX, screenY, &srcRect);
// 	if (m_Enemy) {
// 		m_Enemy->RenderSoul(gRenderer);
// 	}
// }

void Entity::Update(float deltaTime, Camera CameraRect, SDL_Rect PlayerPos)
{
	SDL_Rect srcRect;
	if (!freezeOverworldActors) {

		if (m_isLerping) { // used for CutScenes. this should not interfere with Enemy/NPC.Update() calls.
			float dx = m_targetPosition.x - m_PosX;
			float dy = m_targetPosition.y - m_PosY;
			float dist = sqrtf(dx*dx + dy*dy);
			float step = m_MoveSpeed * deltaTime;

			if (dist <= step) {
				// Close enough — snap and stop
				m_PosX = m_targetPosition.x;
				m_PosY = m_targetPosition.y;
				m_isLerping = false;
				moving = false; // this is for animation frame stopping. 
			} else {
				// Normalize direction, advance by step
				m_PosX += (dx / dist) * step;
				m_PosY += (dy / dist) * step;
			}
		}



		if (m_Enemy) {
			SDL_Rect bruh = PlayerPos; // need to target center of player
			bruh.x = bruh.x + bruh.w / 2;
			bruh.y = bruh.y + bruh.h / 2;
			bruh.w = bruh.w / 2;
			bruh.w = bruh.h / 2;
			m_Enemy->Update(deltaTime, CameraRect, bruh);
		}
		if (m_NPC) {
			m_NPC->Update(deltaTime, CameraRect, PlayerPos);
		}


        // ---- NEW: named-animation system takes priority if in use ----
        if (!m_Animations.empty() && !m_CurrentAnimation.empty()) {
            Animation& anim = m_Animations[m_CurrentAnimation];
            if (m_AnimPlaying && !m_AnimFinishedFlag && anim.frames.size() > 1) {
                m_AnimFrameTimer += deltaTime * 1000.0f;
                if (m_AnimFrameTimer >= anim.frameDuration) {
                    m_AnimFrameTimer = 0.0f;
                    if (m_AnimFrameIndex + 1 >= (int)anim.frames.size()) {
                        if (anim.loop) m_AnimFrameIndex = 0;
                        else            m_AnimFinishedFlag = true; // holds last frame
                    } else {
                        m_AnimFrameIndex++;
                    }
                }
            }
        }
        // ---- legacy path, unchanged, only runs if no named animation set ----
        else if (moving && !m_AnimationFinished) {
            lastFrameTime += deltaTime * 1000.0f;
            if (lastFrameTime >= frameDuration) {
                currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
                lastFrameTime = 0;
                if (m_PlayAnimationOnce && currentFrameCount == 0) {
                    m_AnimationFinished = true;
                }
            }

            if (m_HasBackLayer) {
                m_BackFrameTime += deltaTime * 1000.0f;
                if (m_BackFrameTime >= m_BackFrameDuration) {
                    m_BackFrameCount = (m_BackFrameCount + 1) % m_BackFrameCountMax;
                    m_BackFrameTime = 0;
                }
            }
        }
    }

    // ---- NEW: pick srcRect from whichever system is active ----
    if (!m_Animations.empty() && !m_CurrentAnimation.empty()) {
        Animation& anim = m_Animations[m_CurrentAnimation];
        srcRect = anim.frames[m_AnimFrameIndex];
    } else {
        srcRect = m_Clips[currentFrameCount]; // your original line
    }

	m_Collider = { m_PosX, m_PosY, 128, 128 }; 
	m_FOV = { (m_PosX + (m_Collider.w / 2)) - ((128*3)/2) , (m_PosY + (m_Collider.h / 2)) - ((128 * 3) / 2), (int)(currentFrame.w * 3), (int)(currentFrame.h * 3)};

	//SDL_RenderDrawRect(gRenderer, &m_Collider);
	// SpriteSheet.render(m_PosX - camX, m_PosY - camY, &srcRect);
	//m_Texture->render(m_PosX - CameraRect.x, m_PosY - CameraRect.y, &srcRect);
//	float scale = gameState.mapScaling;
/////	SDL_Rect enemyScreen = CameraRect.worldToScreen({ m_PosX, m_PosY, srcRect.w, srcRect.h }); // scale should be 1 here because the camera's worldToScreen function already accounts for scaling.
//	printf("Entity render: world=(%.1f,%.1f) screen=(%d,%d) scale=%.1f\n", 
//    m_PosX, m_PosY, enemyScreen.x, enemyScreen.y, scale);
    //SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyScreen);
/////	m_Texture->render(enemyScreen.x, enemyScreen.y, &srcRect);
	int screenX = (m_PosX - CameraRect.x);
	int screenY = (m_PosY - CameraRect.y);

	//RenderTrail(screenX, screenY);      // draws behind, no-op for normal entities
	//m_Texture->render(screenX, screenY, &srcRect);

	if (m_HasBackLayer) {
		SDL_Rect backSrcRect = m_BackClips[m_BackFrameCount];
		SDL_SetTextureAlphaMod(m_Texture->getTexture(), 130); // tune to taste
		m_Texture->render(screenX, screenY, &backSrcRect);
		SDL_SetTextureAlphaMod(m_Texture->getTexture(), 255); // restore before front layer
	}

	SDL_Rect renderQuad = { screenX, screenY, m_Collider.w, m_Collider.h };
	//SDL_RenderCopy(gRenderer, m_Texture->getTexture(), &srcRect, &renderQuad);
	m_Texture->render(screenX, screenY, &srcRect);
	if (m_Enemy) {
		m_Enemy->RenderSoul(gRenderer);
	}
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

/// <summary>
/// Attaches an Enemy Child object. 
/// </summary>
void Entity::setEnemy(std::shared_ptr<Enemy> newChild) {
	//m_Enemy = std::move(newChild);
	m_Enemy = newChild;
}

void Entity::setNPC(std::shared_ptr<NPC> newchild) {
	m_NPC = newchild;
}

void Entity::EnableBackLayer(SDL_Rect* backClips, int frameCountMax, float frameDuration) {
    m_HasBackLayer = true;
    m_BackClips = backClips;
    m_BackFrameCountMax = frameCountMax;
    m_BackFrameDuration = frameDuration;
    m_BackFrameCount = 0;
    m_BackFrameTime = 0.0f;
}


void Entity::AddAnimation(const std::string& name, std::vector<SDL_Rect> frames,
                   float frameDuration, bool loop = true) {
    m_Animations[name] = Animation{ std::move(frames), frameDuration, loop };
}

// restart=true always resets to frame 0; restart=false lets you "re-select"
// the same animation without interrupting it (e.g. calling Update() every frame)
void Entity::PlayAnimation(const std::string& name, bool restart = true) {
    auto it = m_Animations.find(name);
    if (it == m_Animations.end()) {
        printf("\n [!] ERROR: Entity::PlayAnimation() unknown animation '%s'", name.c_str());
        return;
    }
    if (m_CurrentAnimation == name && !restart) {
        m_AnimPlaying = true; // in case it was stopped, resume in place
        return;
    }
    m_CurrentAnimation = name;
    m_AnimFrameIndex   = 0;
    m_AnimFrameTimer   = 0.0f;
    m_AnimPlaying      = true;
    m_AnimFinishedFlag = false;
}

void Entity::StopAnimation()   { m_AnimPlaying = false; } // freezes on current frame
void Entity::ResumeAnimation() { m_AnimPlaying = true;  }

void Entity::SetAnimationSpeed(float ms) {
    auto it = m_Animations.find(m_CurrentAnimation);
    if (it != m_Animations.end()) it->second.frameDuration = ms;
}

bool Entity::IsAnimationFinished() const { return m_AnimFinishedFlag; }
const std::string& Entity::GetCurrentAnimation() const { return m_CurrentAnimation; }