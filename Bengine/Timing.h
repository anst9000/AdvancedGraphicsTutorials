#pragma once

namespace Bengine
{
	class FPSLimiter
	{
	public:
		FPSLimiter();

		void init(float targetFPS);
		void setMaxFPS(float targetFPS);

		void begin();

		// end() will return the current FPS
		float end();

	private:
		void calculatFPS();

		float m_fps = 0.0f;
		float m_maxFPS = 0.0f;
		float m_frameTime = 0.0f;

		unsigned int m_startTicks = 0;
	};
}