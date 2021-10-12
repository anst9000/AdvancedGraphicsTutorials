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

		float _fps;
		float _maxFPS;
		float _frameTime;

		unsigned int _startTicks;
	};
}