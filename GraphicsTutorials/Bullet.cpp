#include "Bullet.h"
#include <Bengine/ResourceManager.h>

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime) :
	m_position(pos),
	m_direction(dir),
	m_speed(speed),
	_lifeTime(lifeTime)
{
}

Bullet::~Bullet()
{
}

void Bullet::draw(Bengine::SpriteBatch& spriteBatch)
{
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static Bengine::GLTexture texture = Bengine::ResourceManager::getTexture("Textures/PNG/CharacterRight_Standing.png");

	Bengine::ColorRGBA8 color( 255, 255, 255, 255 );

	glm::vec4 posAndSize = glm::vec4(m_position.x, m_position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, color);
}

bool Bullet::update()
{
	m_position += m_direction * m_speed;
	_lifeTime--;

	return _lifeTime == 0;
}
