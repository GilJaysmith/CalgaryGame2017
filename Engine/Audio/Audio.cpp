#include "Engine/Pch.h"

#include "Engine/Audio/Audio.h"

#include "sdks/fmod/include/fmod.hpp"


namespace Audio
{
	FMOD::System* fmod;

	void* alloc(unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
	{
		return MemAlloc(MemoryPool::Audio, size, sourcestr, 0);
	}

	void* realloc(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
	{
		return nullptr;
	}

	void free(void *ptr, FMOD_MEMORY_TYPE type, const char *sourcestr)
	{
		MemDelete(ptr);
	}

	std::map<FMOD::Channel*, FMOD::Sound*> sounds_playing;

	void Initialize()
	{
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

		FMOD::Memory_Initialize(
			nullptr,
			0,
			alloc,
			realloc,
			free,
			0
		);

		FMOD::System_Create(&fmod);
		fmod->init(512, FMOD_INIT_NORMAL, 0);
	}

	void Terminate()
	{
		for (auto it : sounds_playing)
		{
			FMOD::Channel* channel = it.first;
			bool is_playing = false;
			channel->isPlaying(&is_playing);
			if (is_playing)
			{
				channel->stop();
				FMOD::Sound* sound = it.second;
				sound->release();
			}
		}
		fmod->release();
		CoUninitialize();
	}

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