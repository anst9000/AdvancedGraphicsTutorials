#include "Bengine.h"
#include <SDL/SDL.h>
#include <GL/glew.h>

namespace Bengine
{

	int init()
	{
		// Initialize SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		// Tell SDL that we want a double buffered window to avoid flickering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		return 0;
	}

}