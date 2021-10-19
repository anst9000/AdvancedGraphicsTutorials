#pragma once

#include "Agent.h"

const float ZOMBIE_SPEED = 1.25f;

class Human;

class Zombie : public Agent
{
public:
	Zombie();
	~Zombie();

	void init( float speed, glm::vec2 position );

	virtual void update(
		const std::vector<std::string>& levelData,
		std::vector<Human*>& humans,
		std::vector<Zombie*>& zombies
	);

private:
	Human* getClosestHuman( std::vector<Human*>& humans );
};

