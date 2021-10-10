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
		int _x;
		int _y;
		int _width;
		int _height;

		GLuint _vboID;

		GLTexture _texture;
	};
}
