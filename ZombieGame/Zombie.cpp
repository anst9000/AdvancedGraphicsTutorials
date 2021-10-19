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
	_speed = speed;
	_position = position;

	_health = 100.0f;

	_color.setColor( 0, 160, 0, 255 );
}

void Zombie::update(
	const std::vector<std::string>& levelData,
	std::vector<Human*>& humans,
	std::vector<Zombie*>& zombies
)
{
	Human* closestHuman = getClosestHuman( humans );

	if ( closestHuman != nullptr )
	{
		glm::vec2 direction = glm::normalize( closestHuman->getPosition() - _position );
		_position += direction * _speed;
	}

	collideWithLevel( levelData );
}

Human* Zombie::getClosestHuman( std::vector<Human*>& humans )
{
	Human* closestHuman = nullptr;
	float smallestDistance = 999999.9f;

	std::for_each( humans.begin(), humans.end(), [&]( Human* h )
		{ 
			glm::vec2 distVec = h->getPosition() - _position;
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
