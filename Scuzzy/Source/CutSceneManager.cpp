#include "Source/CutSceneManager.hpp"
#include "Source/GameState.hpp"  // full definition available — no cycle, this is a .cpp
#include <vector>

#ifndef _WIN32
	#include <SDL2/SDL.h>
#elif defined(_WIN32)
	#include <SDL.h>
#endif

extern GameState gameState;

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
    printf("Waiting for player to advance dialogue...\n");
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
    Mix_PlayChannel(-1, m_Explosion, 0);
    fired = true;
}

bool ExplosionAction::Update(float deltaTime) {
    SDL_Rect srcRect;
    if (fired && !m_AnimationFinished) {
                // Calculates index of frame to use in animation.
                lastFrameTime += deltaTime * 1000.0f; // was 1000
                if (lastFrameTime >= 300) { // frameDuration = 100ms ?????
                    currentFrameCount = (currentFrameCount + 1) % FRAME_COUNT;
                    lastFrameTime = 0;
                    if (currentFrameCount == 0) {
                        m_AnimationFinished = true;

                        return true;
                    }	
                }
        srcRect = m_Clips[currentFrameCount]; // render the sprite at index of animation
        int screenX = (m_Pos.x - gameState.cameraRect.x);
        int screenY = (m_Pos.y - gameState.cameraRect.y);
        //m_Texture->render(screenX, screenY, &srcRect);

        SDL_Rect renderQuad = { screenX, screenY, srcRect.w, srcRect.h };
        //SDL_RenderCopy(gRenderer, m_Texture->getTexture(), &srcRect, &renderQuad);
        m_Texture->render(screenX, screenY, &srcRect);
        
    }
    return false;

}

void ExplosionAction::Exit() {
    printf("esploded action completed.\n");
    //gameState.inCutScene = false; // signal cutscene manager to advance to next action or end cutscene
}
