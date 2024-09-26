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
	Enemy(std::shared_ptr<Entity> entity) : m_Entity(entity) {}
	void Update(float deltaT, SDL_Rect CameraRect, SDL_Rect PlayerPos);
	void move(Vector2f targetPos);
	Vector2f moveEntity(Vector2f pos, float deltaTime, Vector2f target);

	//Entity &m_Entity;
	std::shared_ptr<Entity> m_Entity;
	int m_EnemyID = 0;
};

#endif // ENEMY_H