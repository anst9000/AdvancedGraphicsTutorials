#pragma once

#include <glm/glm.hpp>
#include <Bengine/SpriteBatch.h>

class Bullet
{
public:
	Bullet(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime);
	~Bullet();

	void draw(Bengine::SpriteBatch& spriteBatch);

	// Returns true when we are out of life
	bool update();

private:
	glm::vec2 _position;
	glm::vec2 _direction;
	float _speed;
	int _lifeTime;
};

