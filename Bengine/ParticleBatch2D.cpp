#include "ParticleBatch2D.h"

namespace Bengine
{
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

		particle.life = 1.0f;
		particle.position = position;
		particle.velocity = velocity;
		particle.color = color;
		particle.width = width;
	}

	int ParticleBatch2D::findFreeParticle()
	{
		for ( int p = m_lastFreeParticle; p < m_maxParticles; p++ )
		{
			if ( m_particles[p].life <= 0.0f )
			{
				m_lastFreeParticle = p;
				return p;
			}
		}

		for ( int p = 0; p < m_lastFreeParticle; p++ )
		{
			if ( m_particles[ p ].life <= 0.0f )
			{
				m_lastFreeParticle = p;
				return p;
			}
		}

		// No particles are free, overwrite the first particle
		return 0;
	}


	void ParticleBatch2D::init(
		int maxParticles,
		float decayRate,
		GLTexture texture,
		std::function<void( Particle2D&, float )> updateFunc /* = defaultParticleUpdate */
	)
	{
		m_maxParticles = maxParticles;
		m_particles = new Particle2D[ maxParticles ];

		m_decayRate = decayRate;
		m_texture = texture;

		m_updateFunc = updateFunc;
	}

	void ParticleBatch2D::update( float deltaTime )
	{
		for ( int particle = 0; particle < m_maxParticles; particle++ )
		{
			// Check if it is active
			if ( m_particles[ particle ].life > 0.0f )
			{
				m_updateFunc( m_particles[ particle ], deltaTime );
				m_particles[ particle ].life -= m_decayRate * deltaTime;
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
			if ( p.life > 0.0f )
			{
				glm::vec4 destRect( p.position.x, p.position.y, p.width, p.width );
				spriteBatch->draw( destRect, uvRect, m_texture.id, 0.0f, p.color );
			}
		}
	}
}