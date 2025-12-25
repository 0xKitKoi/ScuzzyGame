#pragma once

#ifndef ENEMY_H
#define ENEMY_H

#include "Source/Entity.hpp"
#include "Source/Math.hpp"
#include "Source/LTexture.hpp"
#include "Source/Projectile.hpp"


//extern class Entity;
class Entity;

class Enemy {
public:
	//Enemy(Entity& p);

	/// <summary>
	/// Enemys derive from Entity. Link the two together like so:
	/// std::shared_ptr<Enemy> child = std::make_shared<Enemy>(entity); // make an enemy object initialized with the entity object
	///	entity->setEnemy(child); // bind the new enemy object to the entity
	/// </summary>
	/// <param name="entity">Entity Object.</param>
	Enemy(std::shared_ptr<Entity> entity) : m_Entity(entity), m_EnemyID(entity->m_EntityID) {}
	void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos);
	void Update(float deltaT, int screenhight, int screenwidth);

	virtual void ResetProjectiles();

	virtual std::string FightActionResponse(int actionIndex);

	void move(Vector2f targetPos);
	Vector2f moveEntity(Vector2f pos, float deltaTime, Vector2f target);

	//Entity &m_Entity;
	std::shared_ptr<Entity> m_Entity;
	int m_EnemyID = 0;
	//std::string m_EnemyFightSprite = "data/Error.png";
	std::shared_ptr<LTexture> m_EnemyFightSpriteSheet;
	std::vector<SDL_Rect> m_EnemySpriteClips;
	std::vector<std::string> m_EnemyDialogue;
	std::vector<std::string> m_Actions;
	std::vector<std::string> m_ActionResponse;
	
	int m_AttackDamage = 0;
	bool alive = true;
	int HP = 5;

	int projectileType = 0; // what type of projectile this enemy uses.
	std::shared_ptr<Projectile> m_EnemyProjectile = nullptr;
	int m_projectileCount = 10;
	std::vector<std::shared_ptr<Projectile>> m_EnemyProjectiles;


	int currentFrameCount = 0;
	Uint32 lastFrameTime = 0;
	Uint32 frameDuration = 100; // Time each frame is displayed (milliseconds)
	int FRAME_COUNT = 0; // number of frames in animation.
	
};

/*
class BoxOfFuckYou : public Enemy {
public:
	BoxOfFuckYou(std::shared_ptr<Entity> entity) : Enemy(entity)
};

class DooDooMartBox : public Enemy {
public:
	DooDooMartBox(std::shared_ptr<Entity> entity) : Enemy(entity) 
};*/


#endif // ENEMY_H