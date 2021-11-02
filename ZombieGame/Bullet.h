#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <Bengine/SpriteBatch.h>

class Agent;
class Human;
class Zombie;

const int BULLET_RADIUS = 5;
const float BULLET_SPEED = 20.0f;

class Bullet
{
public:
	Bullet( glm::vec2 position, glm::vec2 direction, float damage, float speed );
	~Bullet();

	// When update returns true, delete bullet
	bool update( const std::vector<std::string>& levelData, float deltaTime );

	void draw( Bengine::SpriteBatch& spriteBatch );

	bool collideWithAgent( Agent* agent );

	float getDamage() const { return m_damage; }

private:
	bool collideWithLevel( const std::vector<std::string>& levelData );

	glm::vec2 m_position;
	glm::vec2 m_direction;

	float m_damage;
	float m_speed;
};
