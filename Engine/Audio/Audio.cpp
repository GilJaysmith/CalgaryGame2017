#include "Engine/Pch.h"

#include "Engine/Audio/Audio.h"

#include "sdks/fmod/include/fmod.hpp"


namespace Audio
{
	FMOD::System* fmod;

	void Initialize()
	{
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		FMOD::System_Create(&fmod);
		fmod->init(512, FMOD_INIT_NORMAL, 0);
	}

	void Terminate()
	{
		fmod->release();
		CoUninitialize();
	}

	std::map<FMOD::Channel*, FMOD::Sound*> sounds_playing;

	void Update(const Time& time)
	{
		fmod->update();
		for (auto it = sounds_playing.begin(); it != sounds_playing.end(); )
		{
			FMOD::Channel* channel = it->first;
			bool is_playing = true;
			channel->isPlaying(&is_playing);
			if (!is_playing)
			{
				FMOD::Sound* sound = it->second;
				sound->release();
				it = sounds_playing.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void PlaySound(const std::string& sound_name)
	{
		std::string sound_path = "Data\\Audio\\" + sound_name;
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT result = fmod->createSound(sound_path.c_str(), FMOD_CREATESAMPLE, nullptr, &sound);
		if (result == FMOD_OK)
		{
			FMOD::Channel* channel = nullptr;
			result = fmod->playSound(sound, nullptr, false, &channel);
			if (result == FMOD_OK)
			{
				auto it = sounds_playing.find(channel);
				if (it != sounds_playing.end())
				{
					it->second->release();
				}
				sounds_playing[channel] = sound;
			}
			else
			{
				Logging::Log("Audio", "Sound sample" + sound_path + " didn't start playing");
				sound->release();
			}
		}
	}
}