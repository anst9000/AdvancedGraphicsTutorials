#include "Window.h"
#include "Errors.h"

namespace Bengine
{

	Window::Window()
	{
	}

	Window::~Window()
	{
	}

	int Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;

		if (currentFlags & INVISIBLE)
		{
			flags |= SDL_WINDOW_HIDDEN;
		}

		if (currentFlags & FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if (currentFlags & BORDERLESS)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		// Open an SDL window
		_sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

		if (_sdlWindow == nullptr)
		{
			fatalError("SDL Window could not be created!");
		}

		// Set up OpenGL context
		SDL_GLContext glContext = SDL_GL_CreateContext(_sdlWindow);
		if (glContext == nullptr)
		{
			fatalError("SDL_GL context could not be created!");
		}

		// Set up glew (optional but recommended)
		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			fatalError("Could not initialize glew!");
		}

		// Check the OpenGL Version
		std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// Set VSYNC disabled
		SDL_GL_SetSwapInterval(0);

		return 0;
	}

	void Window::swapBuffer()
	{
		// Swap our buffer and draw everything to the screen
		SDL_GL_SwapWindow(_sdlWindow);
	}

}