#include "Timing.h"

#include <SDL/SDL.h>

namespace Bengine
{
	FPSLimiter::FPSLimiter()
	{
		// Empty
	}

	void FPSLimiter::init(float maxFPS)
	{
		setMaxFPS(maxFPS);
	}

	void FPSLimiter::setMaxFPS(float maxFPS)
	{
		m_maxFPS = maxFPS;
	}

	void FPSLimiter::begin()
	{
		m_startTicks = SDL_GetTicks();
	}

	float FPSLimiter::end()
	{
		calculatFPS();

		float frameTicks = (float)( SDL_GetTicks() - m_startTicks );

		// Limit the FPS to the max FPS
		if (1000.0f / m_maxFPS > frameTicks)
		{
			SDL_Delay( 1000 / (Uint32)(m_maxFPS - frameTicks) );
		}

		return m_fps;
	}

	void FPSLimiter::calculatFPS()
	{
		static const int NUM_SAMPLES = 10;
		static float frameTimes[NUM_SAMPLES];
		static int currentFrame = 0;

		static float prevTicks = (float)SDL_GetTicks();

		float currentTicks;
		currentTicks = (float)SDL_GetTicks();

		m_frameTime = currentTicks - prevTicks;
		frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;

		prevTicks = currentTicks;

		int count;

		currentFrame++;

		if (currentFrame < NUM_SAMPLES)
		{
			count = currentFrame;
		}
		else
		{
			count = NUM_SAMPLES;
		}

		float frameTimeAverage = 0;

		for (int i = 0; i < count; i++)
		{
			frameTimeAverage += frameTimes[i];
		}

		frameTimeAverage /= count;

		if (frameTimeAverage > 0)
		{
			m_fps = 1000.0f / frameTimeAverage;
		}
		else
		{
			m_fps = 60.0f;
		}
	}
}