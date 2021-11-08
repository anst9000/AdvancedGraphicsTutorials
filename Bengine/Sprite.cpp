#include "Sprite.h"
#include "Vertex.h"
#include "ResourceManager.h"

#include <cstddef>

namespace Bengine
{

	Sprite::Sprite()
	{
		// Empty
	}

	Sprite::~Sprite()
	{
		if (m_vboID != 0)
		{
			glDeleteBuffers(1, &m_vboID);
		}
	}

	void Sprite::init(float x, float y, float width, float height, std::string texturePath)
	{
		m_x = (int)x;
		m_y = (int)y;
		m_width = (int)width;
		m_height = (int)height;

		m_texture = ResourceManager::getTexture(texturePath);

		// Check if VBO has been created or not
		if (m_vboID == 0)
		{
			glGenBuffers(1, &m_vboID);
		}

		Vertex vertexData[6] = {};

		// First triangle
		vertexData[0].setPosition(x + width, y + height);
		vertexData[0].setUV(1.0f, 1.0f);

		vertexData[1].setPosition(x, y + height);
		vertexData[1].setUV(0.0f, 1.0f);

		vertexData[2].setPosition(x, y);
		vertexData[2].setUV(0.0f, 0.0f);

		// Second triangle
		vertexData[3].setPosition(x, y);
		vertexData[3].setUV(0.0f, 0.0f);

		vertexData[4].setPosition(x + width, y);
		vertexData[4].setUV(1.0f, 0.0f);

		vertexData[5].setPosition(x + width, y + height);
		vertexData[5].setUV(1.0f, 1.0f);

		for (int i = 0; i < 6; i++)
		{
			vertexData[1].setColor(255, 0, 255, 255);
		}

		vertexData[1].setColor(0, 0, 255, 255);
		vertexData[4].setColor(0, 255, 0, 255);

		// Bind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		// Upload the buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		// Unbind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Sprite::draw()
	{
		glBindTexture(GL_TEXTURE_2D, m_texture.id);

		// Bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);


		// Draw the 6 vertices on the screen
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Disable the vertex attrib array. Not optional.
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}