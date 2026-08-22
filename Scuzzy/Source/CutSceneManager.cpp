#include "Source/CutSceneManager.hpp"
#include "Source/GameState.hpp"  // full definition available — no cycle, this is a .cpp
#include <vector>

#ifndef _WIN32
	#include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#elif defined(_WIN32)
	#include <SDL.h>
#endif

extern GameState gameState;
extern Mix_Chunk* gExplosionSound;


// Orchestrator class managing the cutscene flow

    void CutsceneManager::AddAction(std::unique_ptr<CutsceneAction> action) {
        m_Actions.push_back(std::move(action));
    }

    void CutsceneManager::StartCutscene() {
        if (m_Actions.empty()) return;
        m_IsActive = true;
        m_CurrentActionIndex = 0;
        printf("--- Player controls disabled. Cutscene Started. ---\n");
        m_Actions[m_CurrentActionIndex]->Enter();
    }

    void CutsceneManager::Update(float deltaTime) {
        if (!m_IsActive) return;
        if (m_CurrentActionIndex >= m_Actions.size()) {
            m_IsActive = false;
            return;
        }

        if (m_Actions[m_CurrentActionIndex]->Update(deltaTime)) {
            m_Actions[m_CurrentActionIndex]->Exit();
            m_CurrentActionIndex++;
            
            if (m_CurrentActionIndex < m_Actions.size()) {
                m_Actions[m_CurrentActionIndex]->Enter();
            } else {
                m_IsActive = false;
                printf("--- Cutscene Finished. Player controls restored. ---\n");
                gameState.inCutScene = false; // Signal that cutscene is done
            }
        }
    }

    
    void CutsceneManager::Render() {
        if (!m_IsActive) return;
        if (m_CurrentActionIndex < m_Actions.size()) {
            m_Actions[m_CurrentActionIndex]->Render();
        }
    }





DialogueAction::DialogueAction(GameState& gs, const std::vector<std::string>& lines)
    : m_GameState(gs), m_DialogueLines(lines) {}

void DialogueAction::Enter() {
    if (!m_DialogueLines.empty()) {
        m_GameState.Text = m_DialogueLines;
        m_GameState.textIndex = 0;
        m_GameState.currentCharIndex = 1;
        m_GameState.textTimer = 0.0f;
        m_GameState.textAnimating = true;
        m_GameState.currentDisplayText = m_GameState.Text.empty() ? "" : m_GameState.Text[0].substr(0, 1);
        m_GameState.shouldAnimateText = true;
        m_GameState.textAvailable = true;
    }
}

bool DialogueAction::Update(float deltaTime) {
    //printf("Waiting for player to advance dialogue...\n");
    if (m_GameState.textAnimating) return false;   // still typing out
    if (m_GameState.textAvailable) return false;   // waiting for player to press confirm

    // player confirmed — advance
    m_CurrentLine++;
    if (m_CurrentLine < m_DialogueLines.size()) {
        m_GameState.Text = { m_DialogueLines[m_CurrentLine] };
        m_GameState.textIndex = 0;
        m_GameState.currentCharIndex = 1;
        m_GameState.textTimer = 0.0f;
        m_GameState.textAnimating = true;
        m_GameState.currentDisplayText = m_GameState.Text[0].substr(0, 1);
        m_GameState.textAvailable = true;   // re-arm for next line
        return false;
    }
    return true;  // all lines done
}

void DialogueAction::Exit() {
    printf("Dialogue action completed.\n");
    //gameState.inCutScene = false; // signal cutscene manager to advance to next action or end cutscene
}


ExplosionAction::ExplosionAction(Mix_Chunk* explosionsound, std::shared_ptr<LTexture> texture, int frameCount, std::vector<SDL_Rect> clips, Vector2f position)
 : m_Explosion(explosionsound), m_Texture(texture), FRAME_COUNT(frameCount), m_Clips(clips), m_Pos(position) { 
    m_CollisionBox = {int(m_Pos.x), int(m_Pos.y), m_Clips[0].w,m_Clips[0].h };
 }

void ExplosionAction::Enter() {
    //play explosion sound, let the entity render itself fast as possible.
    //Mix_PlayChannel(-1, m_Explosion, 0);
    Mix_PlayChannel(-1, gExplosionSound, 0);
    fired = true;
}

// bool ExplosionAction::Update(float deltaTime) {
//     SDL_Rect srcRect;
//     if (fired && !m_AnimationFinished) {
//                 //printf("Explosion animation playing...\n");
//                 // Calculates index of frame to use in animation.
//                 lastFrameTime += deltaTime * 1000.0f; // was 1000
//                 if (lastFrameTime >= 300) { // frameDuration = 100ms ?????
//                     currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
//                     lastFrameTime = 0;
//                     if (currentFrameCount == 0) {
//                         //printf("Explosion animation finished.\n");
//                         m_AnimationFinished = true;

//                         return true;
//                     }	
//                 }
//         srcRect = m_Clips[currentFrameCount]; // render the sprite at index of animation
//         int screenX = (m_Pos.x - gameState.cameraRect.x);
//         int screenY = (m_Pos.y - gameState.cameraRect.y);
//         //m_Texture->render(screenX, screenY, &srcRect);

//         SDL_Rect renderQuad = { screenX, screenY, srcRect.w, srcRect.h };
//         //SDL_Rect renderQuad = { m_Pos.x, m_Pos.y, srcRect.w, srcRect.h };
//         SDL_RenderCopy(gRenderer, m_Texture->getTexture(), &srcRect, &renderQuad);
//         if (!m_Texture->getTexture()) {
//             printf("Explosion texture is NULL!\n");
//         }
//         int texW, texH;
//         SDL_QueryTexture(m_Texture->getTexture(), NULL, NULL, &texW, &texH);
//         printf("Explosion texture size: %d x %d\n", texW, texH);
//         printf("Rendering explosion frame %d at position (%d, %d, %d, %d)\n", currentFrameCount, renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h);
//         printf("Camera position: (%d, %d)\n", gameState.cameraRect.x, gameState.cameraRect.y);
//         printf("Explosion world position: (%f, %f)\n", m_Pos.x, m_Pos.y);
//         printf("Explosion screen position: (%d, %d)\n", screenX, screenY);
//         printf("SrcRect: (%d, %d, %d, %d)\n", srcRect.x, srcRect.y, srcRect.w, srcRect.h);
//         //m_Texture->render(m_Pos.x, m_Pos.y, &srcRect);
//         SDL_RenderPresent(gRenderer); // Force update to show explosion frames immediately
        
//     }
//     return false;

//}

bool ExplosionAction::Update(float deltaTime) {
    if (fired && !m_AnimationFinished) {
        lastFrameTime += deltaTime * 1000.0f;
        if (lastFrameTime >= 100) {
            currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
            lastFrameTime = 0;
            if (currentFrameCount == 0) {
                m_AnimationFinished = true;
                return true;
            }
        }
    }
    return false;
}

// void ExplosionAction::Render() {
//     if (!fired || m_AnimationFinished) return;

//     SDL_Rect srcRect = m_Clips[currentFrameCount];
//     int screenX = m_Pos.x - gameState.cameraRect.x;
//     int screenY = m_Pos.y - gameState.cameraRect.y;
//     SDL_Rect renderQuad = { screenX, screenY, srcRect.w, srcRect.h };

//     SDL_RenderCopy(gRenderer, m_Texture->getTexture(), &srcRect, &renderQuad);
// }
void ExplosionAction::Render() {
    if (!fired || m_AnimationFinished) return;

    SDL_Rect srcRect = m_Clips[currentFrameCount];
    float scale = 3.0f;

    int scaledW = static_cast<int>(srcRect.w * scale);
    int scaledH = static_cast<int>(srcRect.h * scale);

    int screenX = (m_Pos.x - gameState.cameraRect.x) - (scaledW - srcRect.w) / 2;
    int screenY = (m_Pos.y - gameState.cameraRect.y) - (scaledH - srcRect.h) / 2;

    m_Texture->render(screenX, screenY, &srcRect, 0.0, nullptr, SDL_FLIP_NONE, scale);
}

void ExplosionAction::Exit() {
    printf("esploded action completed.\n");
    //gameState.inCutScene = false; // signal cutscene manager to advance to next action or end cutscene
}


SpriteShowAction::SpriteShowAction(std::shared_ptr<LTexture> texture, std::vector<SDL_Rect> clips, Vector2f position, int frameCount, int durationMs, bool loop)
    : m_Texture(texture), m_Clips(clips), m_Pos(position), m_FrameCount(frameCount), duration(durationMs), m_loop(loop) {}

void SpriteShowAction::Enter() {
    m_CurrentFrame = 0;
    m_AnimationFinished = false;
    m_ElapsedMs = 0.0f;
    m_FrameElapsedMs = 0.0f;
}
void SpriteShowAction::Render() {
    if (m_AnimationFinished || !m_Texture || m_Clips.empty()) return;

    const int frameCount = std::min(m_FrameCount, static_cast<int>(m_Clips.size()));
    if (frameCount <= 0) return;

    SDL_Rect srcRect = m_Clips[m_CurrentFrame];
    const int screenX = static_cast<int>(m_Pos.x - gameState.cameraRect.x);
    const int screenY = static_cast<int>(m_Pos.y - gameState.cameraRect.y);
    m_Texture->render(screenX, screenY, &srcRect);
}
void SpriteShowAction::Exit() {
    printf("SpriteShowAction completed.\n");
    //gameState.inCutScene = false; // signal cutscene manager to advance to next action or end cutscene
}
bool SpriteShowAction::Update(float deltaTime) {
    if (m_AnimationFinished) return true;

    const int frameCount = std::min(m_FrameCount, static_cast<int>(m_Clips.size()));
    if (duration <= 0 || frameCount <= 0) {
        m_AnimationFinished = true;
        return true;
    }

    const float deltaMs = deltaTime * 1000.0f;
    m_ElapsedMs += deltaMs;
    m_FrameElapsedMs += deltaMs;

    // Advance at a fixed rate. A non-looping animation holds its last frame
    // until the requested display duration ends; a looping one wraps instead.
    while (m_FrameElapsedMs >= m_FrameDurationMs) {
        m_FrameElapsedMs -= m_FrameDurationMs;
        if (m_CurrentFrame + 1 < frameCount) {
            ++m_CurrentFrame;
        } else if (m_loop) {
            m_CurrentFrame = 0;
        }
    }

    if (m_ElapsedMs >= duration) {
        m_AnimationFinished = true;
        return true;
    }

    return false;
}


SoundEffectAction::SoundEffectAction(Mix_Chunk* soundEffect, bool repeat, int repeatCount)
    : m_SoundEffect(soundEffect), m_repeat(repeat), m_repeatCount(repeatCount) {}


void SoundEffectAction::Enter() {
    m_Played = false;
    if (m_SoundEffect) {
        // SDL_mixer performs repeats asynchronously. The loops argument is
        // the number of additional plays, matching m_repeatCount's contract.
        const int loops = m_repeat ? m_repeatCount : 0;
        Mix_PlayChannel(-1, m_SoundEffect, loops);
    }
    m_Played = true;
}
bool SoundEffectAction::Update(float deltaTime) {
    // The sound keeps playing on SDL_mixer after this action completes.
    // Do not keep the cutscene stuck here or start overlapping copies.
    return m_Played;
}
// void SoundEffectAction::Render() {
//     // Sound effects do not render anything.
// }
void SoundEffectAction::Exit() {
    printf("SoundEffectAction completed.\n");
    //gameState.inCutScene = false; // signal cutscene manager to advance to next action or end cutscene
}


void HidePlayerAction::Enter() {
	gameState.player->m_Invisible = true;
}

void HidePlayerAction::Exit() {
	gameState.player->m_Invisible = false;
};



void MovePlayerAction::Enter() {
    //m_StartPosition = Vector2f {float(gameState.player->GetPosX()), float(gameState.player->GetPosY())};
    //printf("MovePlayerAction: Start Position: (%f, %f)\n", m_StartPosition.x, m_StartPosition.y);
    //printf("MovePlayerAction: Target Position: (%f, %f)\n", m_TargetPosition.x, m_TargetPosition.y);
    m_ReturnPosition = Vector2f {float(gameState.player->GetPosX()), float(gameState.player->GetPosY())};
    gameState.player->m_PosX =
        static_cast<int>(m_StartPosition.x);

    gameState.player->m_PosY =
        static_cast<int>(m_StartPosition.y);
    m_AnimationFinished = false;
    lastFrameTime = 0;
    currentFrame = 0;
}


bool MovePlayerAction::Update(float deltaTime) {
    deltaTime = gameState.deltaTime;
	Vector2f direction = m_TargetPosition - m_StartPosition;
    // printf("MovePlayerAction: Start Position: (%f, %f)\n", m_StartPosition.x, m_StartPosition.y);
    // printf("MovePlayerAction: Target Position: (%f, %f)\n", m_TargetPosition.x, m_TargetPosition.y);
    // //printf("MovePlayerAction: Player Position: (%d, %d)\n", gameState.player->GetPosX(), gameState.player->GetPosY());
    // printf("MovePlayerAction: Direction Vector: (%f, %f)\n", direction.x, direction.y);
    // printf("MovePlayerAction: Delta Time: %f\n", deltaTime);
    

    


    float distance = direction.Length();
    // printf("MovePlayerAction: Distance: %f\n", distance);
    if (distance == 0.0f) {
        printf("MovePlayerAction: FINISHED - zero distance\n");
        return true;
    }
	// if (distance == 0.0f) return true;
	Vector2f normalizedDirection = direction.Normalized();
	float moveDistance = m_Speed * deltaTime;
	Vector2f newPosition = Vector2f {float(gameState.player->GetPosX()), float(gameState.player->GetPosY())} + normalizedDirection * moveDistance;
    // printf(
    //     "PLAYER BEFORE: (%d, %d) | NEW: (%f, %f)\n",
    //     gameState.player->m_PosX,
    //     gameState.player->m_PosY,
    //     newPosition.x,
    //     newPosition.y
    // );

	// Check if the player has reached or passed the target position
	if ((normalizedDirection.x > 0 && newPosition.x >= m_TargetPosition.x) ||
		(normalizedDirection.x < 0 && newPosition.x <= m_TargetPosition.x) ||
		(normalizedDirection.y > 0 && newPosition.y >= m_TargetPosition.y) ||
		(normalizedDirection.y < 0 && newPosition.y <= m_TargetPosition.y)) {
		gameState.player->m_PosX = static_cast<int>(m_TargetPosition.x);
		gameState.player->m_PosY = static_cast<int>(m_TargetPosition.y);
		m_AnimationFinished = true;
        // printf("MovePlayerAction: RETURNING TRUE - reached target\n");
		return true;
	}
	else {
		gameState.player->m_PosX = static_cast<int>(newPosition.x);
		gameState.player->m_PosY = static_cast<int>(newPosition.y);
        // printf(
        //     "PLAYER AFTER: (%d, %d)\n",
        //     gameState.player->m_PosX,
        //     gameState.player->m_PosY
        // );
		
		// LERP to position for smoother movement
		//gameState.player->m_PosX = static_cast<int>(gameState.player->m_PosX + (newPosition.x - gameState.player->m_PosX) * 0.1f);
		//gameState.player->m_PosY = static_cast<int>(gameState.player->m_PosY + (newPosition.y - gameState.player->m_PosY) * 0.1f);

		// ANIMATE the player sprite, using the given clippings.
		lastFrameTime += deltaTime * 1000.0f;
		if (!gameState.inFight) {
			frameDuration = 100;
		}
		else { frameDuration = 400; }
		while (lastFrameTime >= frameDuration) {
			currentFrame = (currentFrame + 1) % 4;
			lastFrameTime -= frameDuration;  // Subtract instead of setting to 0
		}
        printf("FRAME: %d\n", currentFrame);
        gameState.player->m_CutsceneClip = m_Clips[currentFrame];

        printf(
            "FRAME %d: clip = {%d, %d, %d, %d}\n",
            currentFrame,
            m_Clips[currentFrame].x,
            m_Clips[currentFrame].y,
            m_Clips[currentFrame].w,
            m_Clips[currentFrame].h
        );
		//render that...?
		//gameState.player->CurrentSprite.render(gameState.player->m_PosX - gameState.cameraRect.x, gameState.player->m_PosY - gameState.cameraRect.y, &m_Clips[currentFrame]);
        //gameState.player->render(gameState.player->m_PosX - gameState.cameraRect.x, gameState.player->m_PosY - gameState.cameraRect.y); //, &m_Clips[currentFrame]);
        //gameState.player->SpriteSheet.render(gameState.player->m_PosX - gameState.cameraRect.x, gameState.player->m_PosY - gameState.cameraRect.y, &m_Clips[currentFrame]);

	}
        
    //printf("MovePlayerAction: RETURNING FALSE\n");
    return false;
}

void MovePlayerAction::Exit() {
	// return the player to its original position if specified

	if (m_ReturnPosition.x != 0.0f || m_ReturnPosition.y != 0.0f) {
		gameState.player->m_PosX = static_cast<int>(m_ReturnPosition.x);
		gameState.player->m_PosY = static_cast<int>(m_ReturnPosition.y);
	}
	//gameState.player->m_Invisible = false; // ensure player is visible after cutscene..?
    printf("MovePlayerAction completed.\n");
}
