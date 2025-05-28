#pragma once

#ifndef NPC_H
#define NPC_H

#include "Source/Entity.hpp"
#include "Source/Math.hpp"
#include "Source/GameState.hpp"


//extern class Entity;
class Entity;

class NPC {
public:

	NPC(std::shared_ptr<Entity> entity, std::vector<std::string> dialogue) : m_Entity(entity), m_Dialogue(dialogue) {
	}
	

	//void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos);
	virtual void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos) {
		// detect if player check box collides with NPC hitbox?
		// Player can detect this, how do I tell the NPC this?
		if (m_checked) {
			gameState.Text.clear();
			gameState.Text = m_Dialogue;
			gameState.textAvailable;
			gameState.callbackNPC = this;
			m_checked = false;

			//gameState.Text.clear();
			//gameState.Text.push_back("Who are you talking to..?");
			gameState.textIndex = 0;
			gameState.currentCharIndex = 1; // offset because i need a char to start the animation.
			gameState.textTimer = 0.0f;
			gameState.textAnimating = true;
			gameState.currentDisplayText = gameState.Text[0][0];// "";
			gameState.shouldAnimateText = true;  // This is dialogue, so animate it
			gameState.textAvailable = true;


		}
	}
	/*
	void move(Vector2f targetPos);
	Vector2f moveEntity(Vector2f pos, float deltaTime, Vector2f target);
	*/
	
	std::shared_ptr<Entity> m_Entity;

	bool m_checked = false;
	int m_TalkedTo = 0;
	std::vector<std::string> m_Dialogue;
	std::string m_room;
	Vector2f m_Location;


	// Virtual function for handling the player's choice
	virtual void handleChoice(int choice) {
		std::cout << "NPC default handling: " << m_Dialogue[choice] << std::endl;
	}

	virtual ~NPC() = default;  // Virtual destructor
	
};

#endif // NPC_H