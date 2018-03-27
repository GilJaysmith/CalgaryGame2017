#pragma once

class Time;

typedef void* AudioHandle;


namespace Audio
{
	void Initialize();

	void Terminate();

	void SetListenerPosition(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward);
	void Update(const Time& time);

	AudioHandle PlaySound(const std::string& sound_name, bool looping = false);
	AudioHandle PlaySound(const std::string& sound_name, const glm::vec3& position, bool looping = false);

	void Pause(AudioHandle handle);
	void Resume(AudioHandle handle);
	void Stop(AudioHandle handle);

	Time GetTimeElapsed(AudioHandle handle);
	Time GetTimeRemaining(AudioHandle handle);
	void SetVolume(AudioHandle handle, float volume);
	void SetPitch(AudioHandle handle, float pitch);
	void SetLooping(AudioHandle handle);
	void SetPosition(AudioHandle handle, const glm::vec3& position);
}