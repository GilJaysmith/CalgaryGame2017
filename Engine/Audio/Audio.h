#pragma once

class Time;


namespace Audio
{
	void Initialize();

	void Terminate();

	void Update(const Time& time);

	void PlaySound(const std::string& sound_name);
}