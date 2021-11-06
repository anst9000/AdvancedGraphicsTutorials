#pragma once

#include <string>
#include <GL/glew.h>
#include "GLTexture.h"

namespace Bengine
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void init(float x, float y, float width, float height, std::string texturePath);
		void draw();

	private:
		int m_x = 0;
		int m_y = 0;
		int m_width = 0;
		int m_height = 0;

		GLuint m_vboID = 0;

		GLTexture m_texture { 0 };
	};
}
