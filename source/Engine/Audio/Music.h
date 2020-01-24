#pragma once


class Time;

namespace Music
{
	void Initialize();
	void Terminate();

	void Update(Time& update_time);

	void LoadPlaylist(const std::string& playlist_filename);

	void StartPlaying();
	void PausePlaying();
	void ResumePlaying();
	void StopPlaying();
}