#include "Engine/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/GameStates/Time.h"

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
			nullptr,
			nullptr,
			nullptr,
			0
		);

		FMOD::System_Create(&fmod);
		fmod->init(512, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
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

	void SetListenerPosition(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward)
	{
		FMOD_VECTOR fmod_pos;
		fmod_pos.x = position.x;
		fmod_pos.y = position.y;
		fmod_pos.z = position.z;
		FMOD_VECTOR fmod_vel;
		fmod_vel.x = velocity.x;
		fmod_vel.y = velocity.y;
		fmod_vel.z = velocity.z;
		FMOD_VECTOR fmod_forward;
		fmod_forward.x = forward.x;
		fmod_forward.y = forward.y;
		fmod_forward.z = forward.z;
		fmod->set3DListenerAttributes(0, &fmod_pos, &fmod_vel, &fmod_forward, nullptr);
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

	AudioHandle PlaySound(const std::string& sound_name, bool looping)
	{
		std::string sound_path = "Data\\Audio\\" + sound_name;
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT result = fmod->createSound(sound_path.c_str(), FMOD_CREATESAMPLE | FMOD_ACCURATETIME | (looping ? FMOD_LOOP_NORMAL : 0), nullptr, &sound);
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
				return channel;
			}
			else
			{
				Logging::Log("Audio", "Sound sample" + sound_path + " didn't start playing");
				sound->release();
				return nullptr;
			}
		}
		return nullptr;
	}

	AudioHandle PlaySound(const std::string& sound_name, const glm::vec3& position, bool looping)
	{
		std::string sound_path = "Data\\Audio\\" + sound_name;
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT result = fmod->createSound(sound_path.c_str(), FMOD_3D | (looping ? FMOD_LOOP_NORMAL : 0), nullptr, &sound);
		if (result == FMOD_OK)
		{
			FMOD::Channel* channel = nullptr;
			result = fmod->playSound(sound, nullptr, true, &channel);
			if (result == FMOD_OK)
			{
				auto it = sounds_playing.find(channel);
				if (it != sounds_playing.end())
				{
					it->second->release();
				}
				sounds_playing[channel] = sound;
				FMOD_VECTOR pos;
				pos.x = position.x;
				pos.y = position.y;
				pos.z = position.z;
				channel->set3DAttributes(&pos, nullptr);
				channel->setPaused(false);
				return channel;
			}
			else
			{
				Logging::Log("Audio", "Sound sample" + sound_path + " didn't start playing");
				sound->release();
				return nullptr;
			}
		}
		return nullptr;
	}

	void Pause(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			channel->setPaused(true);
		}
	}

	void Resume(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			channel->setPaused(false);
		}
	}

	void Stop(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			channel->stop();
		}
	}

	Time GetTimeElapsed(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			unsigned int position;
			channel->getPosition(&position, FMOD_TIMEUNIT_MS);

			return Time::fromMilliseconds(float(position));
		}
		return Time::fromSeconds(-1.0f);
	}

	Time GetTimeRemaining(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			unsigned int length;
			it->second->getLength(&length, FMOD_TIMEUNIT_MS);

			unsigned int position;
			channel->getPosition(&position, FMOD_TIMEUNIT_MS);

			return Time::fromMilliseconds(float(length - position));
		}
		return Time::fromSeconds(-1.0f);
	}

	void SetVolume(AudioHandle handle, float volume)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			channel->setVolume(volume);
		}
	}

	void SetPitch(AudioHandle handle, float pitch)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			channel->setPitch(pitch);
		}
	}

	void SetLooping(AudioHandle handle)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			unsigned int length;
			it->second->getLength(&length, FMOD_TIMEUNIT_PCM);
			channel->setLoopCount(-1);
			channel->setLoopPoints(0, FMOD_TIMEUNIT_PCM, length - 1, FMOD_TIMEUNIT_PCM);
		}
	}

	void SetPosition(AudioHandle handle, const glm::vec3& position)
	{
		FMOD::Channel* channel = static_cast<FMOD::Channel*>(handle);
		auto it = sounds_playing.find(channel);
		if (it != sounds_playing.end())
		{
			FMOD_VECTOR pos;
			pos.x = position.x;
			pos.y = position.y;
			pos.z = position.z;
			channel->set3DAttributes(&pos, nullptr);
		}
	}
}