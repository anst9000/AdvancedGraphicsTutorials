#pragma once

#include "Agent.h"

const float HUMAN_SPEED = 1.0f;

class Human : public Agent
{
public:
	Human();
	virtual ~Human();

	void init( float speed, glm::vec2 position );

	virtual void update(
		const std::vector<std::string>& levelData,
		std::vector<Human*>& humans,
		std::vector<Zombie*>& zombies,
		float deltaTime) override;

private:
	int m_frames { 0 };
};

