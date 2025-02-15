#pragma once

#ifndef ENEMY_H
#define ENEMY_H

#include "Source/Entity.hpp"
#include "Source/Math.hpp"


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
	void move(Vector2f targetPos);
	Vector2f moveEntity(Vector2f pos, float deltaTime, Vector2f target);

	//Entity &m_Entity;
	std::shared_ptr<Entity> m_Entity;
	int m_EnemyID = 0;
	std::string m_EnemyFightSprite = "data/Error.png";

	std::vector<std::string> m_EnemyDialogue;
	int m_AttackDamage = 0;

	
};

#endif // ENEMY_H