#pragma once

class Time;


namespace Audio
{
	void Initialize();

	void Terminate();

	void SetListenerPosition(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward);
	void Update(const Time& time);

	void PlaySound(const std::string& sound_name);
	void PlaySound(const std::string& sound_name, const glm::vec3& position);
}