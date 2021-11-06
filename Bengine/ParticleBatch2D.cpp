#include "ParticleBatch2D.h"

namespace Bengine
{
	void Particle2D::update( float deltaTime )
	{
		m_position += m_velocity * deltaTime;
	}


	ParticleBatch2D::ParticleBatch2D()
	{
		// Empty
	}

	ParticleBatch2D::~ParticleBatch2D()
	{
		delete[] m_particles;
	}

	void ParticleBatch2D::addParticle( 
		const glm::vec2& position, 
		const glm::vec2& velocity, 
		const Bengine::ColorRGBA8& color,
		float width )
	{
		int particleIndex = findFreeParticle();

		auto& particle = m_particles[ particleIndex ];

		particle.m_life = 1.0f;
		particle.m_position = position;
		particle.m_velocity = velocity;
		particle.m_color = color;
		particle.m_width = width;
	}

	int ParticleBatch2D::findFreeParticle()
	{
		for ( int p = m_lastFreeParticle; p < m_maxParticles; p++ )
		{
			if ( m_particles[p].m_life <= 0.0f )
			{
				m_lastFreeParticle = p;
				return p;
			}
		}

		for ( int p = 0; p < m_lastFreeParticle; p++ )
		{
			if ( m_particles[ p ].m_life <= 0.0f )
			{
				m_lastFreeParticle = p;
				return p;
			}
		}

		// No particles are free, overwrite the first particle
		return 0;
	}


	void ParticleBatch2D::init( int maxParticles, float decayRate, GLTexture texture )
	{
		m_maxParticles = maxParticles;
		m_particles = new Particle2D[ maxParticles ];

		m_decayRate = decayRate;
		m_texture = texture;
	}

	void ParticleBatch2D::update( float deltaTime )
	{
		for ( int particle = 0; particle < m_maxParticles; particle++ )
		{
			// Check if it is active
			if ( m_particles[ particle ].m_life > 0.0f )
			{
				m_particles[ particle ].update( deltaTime );
				m_particles[ particle ].m_life -= m_decayRate * deltaTime;
			}
		}
	}

	void ParticleBatch2D::draw( SpriteBatch* spriteBatch )
	{
		glm::vec4 uvRect( 0.0f, 0.0f, 1.0f, 1.0f );
		for ( int particle = 0; particle < m_maxParticles; particle++ )
		{
			auto& p = m_particles[ particle ];

			// Check if it is active
			if ( p.m_life > 0.0f )
			{
				glm::vec4 destRect( p.m_position.x, p.m_position.y, p.m_width, p.m_width );
				spriteBatch->draw( destRect, uvRect, m_texture.id, 0.0f, p.m_color );
			}
		}
	}
}