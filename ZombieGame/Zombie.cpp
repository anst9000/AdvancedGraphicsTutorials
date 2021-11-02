#include "Zombie.h"
#include "Human.h"

#include <algorithm>

Zombie::Zombie()
{
}

Zombie::~Zombie()
{
}

void Zombie::init( float speed, glm::vec2 position )
{
	m_speed = speed;
	m_position = position;

	m_health = 100.0f;

	m_color = Bengine::ColorRGBA8( 0, 160, 0, 255 );
}

void Zombie::update(
	const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies,
	float deltaTime
)
{
	Human* closestHuman = getClosestHuman( humans );

	if ( closestHuman != nullptr )
	{
		glm::vec2 direction = glm::normalize( closestHuman->getPosition() - m_position );
		m_position += direction * m_speed * deltaTime;
	}

	collideWithLevel( levelData );
}

Human* Zombie::getClosestHuman( std::vector<Human*>& humans )
{
	Human* closestHuman = nullptr;
	float smallestDistance = 999999.9f;

	std::for_each( humans.begin(), humans.end(), [&]( Human* h )
		{ 
			glm::vec2 distVec = h->getPosition() - m_position;
			float distance = glm::length( distVec );

			if ( distance < smallestDistance )
			{
				smallestDistance = distance;
				closestHuman = h;
			}
		}
	);

	return closestHuman;
}
