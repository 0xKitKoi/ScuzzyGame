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

	//Entity &m_Entity;
	std::shared_ptr<Entity> m_Entity;
};

#endif // ENEMY_H