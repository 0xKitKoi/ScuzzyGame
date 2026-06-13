#pragma once
#ifndef CUTSCENEMANAGER_H
#define CUTSCENEMANAGER_H

#include <vector>
#include <memory>
#include <cstdio>

#include "Source/Entity.hpp"
#include "Source/GameState.hpp"

// Base class for individual actions within a cutscene
class CutsceneAction {
public:
    virtual ~CutsceneAction() = default;
    virtual void Enter() = 0;
    virtual bool Update(float deltaTime) = 0; // Returns true when completed
    virtual void Exit() = 0;
};

// Orchestrator class managing the cutscene flow
class CutsceneManager {
    std::vector<std::unique_ptr<CutsceneAction>> m_Actions;
    size_t m_CurrentActionIndex = 0;
    bool m_IsActive = false;

public:
    void AddAction(std::unique_ptr<CutsceneAction> action) {
        m_Actions.push_back(std::move(action));
    }

    void StartCutscene() {
        if (m_Actions.empty()) return;
        m_IsActive = true;
        m_CurrentActionIndex = 0;
        printf("--- Player controls disabled. Cutscene Started. ---\n");
        m_Actions[m_CurrentActionIndex]->Enter();
    }

    void Update(float deltaTime) {
        if (!m_IsActive) return;

        if (m_Actions[m_CurrentActionIndex]->Update(deltaTime)) {
            m_Actions[m_CurrentActionIndex]->Exit();
            m_CurrentActionIndex++;
            
            if (m_CurrentActionIndex < m_Actions.size()) {
                m_Actions[m_CurrentActionIndex]->Enter();
            } else {
                m_IsActive = false;
                printf("--- Cutscene Finished. Player controls restored. ---\n");
            }
        }
    }
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


#endif // CUTSCENEMANAGER_H