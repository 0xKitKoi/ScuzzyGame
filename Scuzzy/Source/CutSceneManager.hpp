#pragma once
#ifndef CUTSCENEMANAGER_H
#define CUTSCENEMANAGER_H

#include <vector>
#include <memory>
#include <cstdio>

#include "Source/Entity.hpp"
#ifdef _WIN32
	#include <SDL_mixer.h>
#elif defined(__linux__)
	#include "SDL2/SDL_mixer.h"
#endif 




class GameState; // Forward declaration

// Base class for individual actions within a cutscene
class CutsceneAction {
public:
    virtual ~CutsceneAction() = default;
    virtual void Enter() = 0;
    virtual bool Update(float deltaTime) = 0; // Returns true when completed
    virtual void Render() {};
    virtual void Exit() = 0;
};

// Orchestrator class managing the cutscene flow
class CutsceneManager {
public:
    std::vector<std::unique_ptr<CutsceneAction>> m_Actions;
    size_t m_CurrentActionIndex = 0;
    bool m_IsActive = false;
    void AddAction(std::unique_ptr<CutsceneAction> action);

    void StartCutscene();

    void Update(float deltaTime);
    void Render();
};




// Concrete Action: Controlling the camera
class MoveCameraAction : public CutsceneAction {
    float m_Duration;
    float m_Elapsed = 0.0f;
public:
    MoveCameraAction(float time) : m_Duration(time) {}
    void Enter() override { printf("Targeting cinematic camera track...\n"); }
    bool Update(float deltaTime) override {
        m_Elapsed += deltaTime;
        printf("Interpolating camera position: %f%%\n", (m_Elapsed / m_Duration) * 100);
        return m_Elapsed >= m_Duration;
    }
    void Exit() override { printf("Camera interpolation finished.\n"); }
};



// Concrete Action: Pausing the sequence without freezing the engine
class DelayAction : public CutsceneAction {
private:
    float m_DelayDuration;
    float m_TimeAccumulator = 0.0f;

public:
    // Pass the desired sleep/delay time in seconds
    DelayAction(float seconds) : m_DelayDuration(seconds) {}

    void Enter() override {
        m_TimeAccumulator = 0.0f; // Reset timer when the action starts
        printf("[Delay] Waiting for %f seconds...\n", m_DelayDuration);
    }

    bool Update(float deltaTime) override {
        m_TimeAccumulator += deltaTime;
        
        // Return true only when the required time has passed
        return m_TimeAccumulator >= m_DelayDuration;
    }

    void Exit() override {
        printf("[Delay] Time up! Moving to next action.\n");
    }
};


class MoveEntityAction : public CutsceneAction {
private:
    Entity* m_Entity;
    Vector2f m_TargetDestination;
    float m_SpeedDuration;

public:
    MoveEntityAction(Entity* entity, Vector2f target, float duration) 
        : m_Entity(entity), m_TargetDestination(target), m_SpeedDuration(duration) {}

    void Enter() override {
        // Tell the actor to start moving
        m_Entity->MoveTo(m_TargetDestination, m_SpeedDuration);
    }

    bool Update(float deltaTime) override {
        // The manager updates the whole game, so the entity is already updating.
        // This action is complete when the entity stops moving.
        return !m_Entity->m_isLerping;
    }

    void Exit() override {
        printf("Entity reached destination.\n");
    }
};


class DialogueAction : public CutsceneAction {
private:
    GameState& m_GameState;
    std::vector<std::string> m_DialogueLines;
    size_t m_CurrentLine = 0;
public:
    DialogueAction(GameState& gs, const std::vector<std::string>& lines);
    void Enter() override;
    bool Update(float deltaTime) override;
    void Exit() override;
};


class ExplosionAction : public CutsceneAction {
public:
    SDL_Rect m_CollisionBox;
    std::shared_ptr<LTexture> m_Texture;
    Mix_Chunk* m_Explosion = NULL;
    bool fired = false;
    bool m_AnimationFinished = false;
    int lastFrameTime = 0;
    int currentFrameCount = 1;
    int FRAME_COUNT = 0;
    SDL_Rect srcRect = {};
    std::vector<SDL_Rect> m_Clips;
    Vector2f m_Pos;

    ExplosionAction(Mix_Chunk* explosionsound, std::shared_ptr<LTexture> texture, int frameCount, std::vector<SDL_Rect> clips, Vector2f position);
    void Enter() override;
    bool Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
};

class SpriteShowAction : public CutsceneAction {
public:
    std::shared_ptr<LTexture> m_Texture;
    SDL_Rect m_SpriteRect;
    Vector2f m_Pos;
    std::vector<SDL_Rect> m_Clips;
    int m_FrameCount;
    int duration; // total display time in milliseconds
    int m_CurrentFrame = 0;
    bool m_loop = false;
    bool m_AnimationFinished = false;
    float m_ElapsedMs = 0.0f;
    float m_FrameElapsedMs = 0.0f;
    float m_FrameDurationMs = 100.0f; // 10 animation frames per second

    SpriteShowAction(std::shared_ptr<LTexture> texture, std::vector<SDL_Rect> clips, Vector2f position, int frameCount, int durationMs, bool loop = false);
    void Enter() override;
    bool Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
};

class SoundEffectAction : public CutsceneAction {
public:
    Mix_Chunk* m_SoundEffect = NULL;
    bool m_repeat = false;
    int m_repeatCount = 0; // how many times to repeat the sound effect. 0 = no repeat, 1 = play twice, etc.
    bool m_Played = false;
    SoundEffectAction(Mix_Chunk* soundEffect, bool repeat, int repeatCount);
    void Enter() override;
    bool Update(float deltaTime) override;
    void Exit() override;
};


class HidePlayerAction : public CutsceneAction {
    float m_Duration;
public:
    HidePlayerAction(float duration) : m_Duration(duration) {}
    //void Enter() override {
    //    gameState.player->m_Invisible = true;
    //};
	void Enter() override;
    bool Update(float deltaTime) override {
        m_Duration -= deltaTime;
        return m_Duration <= 0;
    };
	void Exit() override;
};



class MovePlayerAction : public CutsceneAction {
	Vector2f m_ReturnPosition; // position to return the player to after the cutscene..?
	Vector2f m_StartPosition;
	Vector2f m_TargetPosition;
	float m_Speed;
	bool m_AnimationFinished = false;
	int lastFrameTime = 0;
	int frameDuration = 100; // milliseconds per frame
	int currentFrame = 0;
	// Sprite sheet clips for animation. 
	std::vector<SDL_Rect> m_Clips;
	MovePlayerAction(Vector2f start, Vector2f target, float speed, std::vector<SDL_Rect> clips, Vector2f returnPosition = Vector2f(0.0f, 0.0f))
		: m_StartPosition(start), m_TargetPosition(target), m_Speed(speed), m_Clips(clips), m_ReturnPosition(returnPosition) { }
	//void Enter(Vector2f position) override; member function declared with 'override' does not override a base class member Oopsies
	void Enter() override;
	bool Update(float deltaTime) override;
	void Exit() override;
};



#endif // CUTSCENEMANAGER_H
