#pragma once

#include <SDL/SDL_mixer.h>
#include <string>
#include <map>

namespace Bengine
{
	class SoundEffect
	{
	public:
		friend class AudioEngine;

		/// <summary>
		///		Plays the effect file
		/// </summary>
		/// <param name="loops">
		///		if loops == -1, loop forever, ohterwise play it loops + 1 times
		/// </param>
		void play( int loops = 0 );

	private:
		Mix_Chunk* m_chunk = NULL;
	};

	class Music
	{
	public:
		friend class AudioEngine;

		/// <summary>
		///		Plays the audio file
		/// </summary>
		/// <param name="loops">
		///		if loops == -1, loop forever, ohterwise play it loops times
		/// </param>
		void play( int loops = 1 );

		static void pause();
		static void stop();
		static void resume();

	private:
		Mix_Music* m_music = NULL;
	};

	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();

		void init();
		void destroy();

		SoundEffect loadSoundEffect(const std::string& filePath);
		Music loadMusic( const std::string& filePath );

	private:
		std::map<std::string, Mix_Chunk*> m_effectMap;		///< Effects cache
		std::map<std::string, Mix_Music*> m_musicMap;		///< Music cache

		bool m_isInitialized = false;
	};
}