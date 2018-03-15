#include "Engine/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Audio/Music.h"
#include "Engine/GameStates/Time.h"

namespace Music
{
	void Initialize()
	{
	}

	void Terminate()
	{
	}

	struct PlaylistTrack
	{
		std::string file;
	};

	std::vector<PlaylistTrack> s_Playlist;
	size_t s_PlaylistIndex;
	AudioHandle s_PreviousTrack;
	AudioHandle s_CurrentTrack;
	bool s_Paused = false;

	AudioHandle PlayTrack(size_t playlist_index)
	{
		PlaylistTrack track = s_Playlist[playlist_index];
		std::string track_filename = "music\\" + track.file;
		AudioHandle audio_handle = Audio::PlaySound(track_filename);
		assert(audio_handle);
		Audio::SetVolume(audio_handle, 0.0f);
		s_Paused = false;
		return audio_handle;
	}

	void Update(Time& update_time)
	{
		// Update current track.
		float current_volume = 1.0f;
		float previous_volume = 1.0f;

		const float FADE_DURATION = 5.0f;

		if (s_CurrentTrack)
		{
			float current_track_time_remaining = Audio::GetTimeRemaining(s_CurrentTrack).toSeconds();
			// Have we just entered the fade time? Initialize the next track.
			if (current_track_time_remaining < FADE_DURATION && s_PreviousTrack == nullptr)
			{
				s_PreviousTrack = s_CurrentTrack;
				s_PlaylistIndex = (s_PlaylistIndex + 1) % s_Playlist.size();
				s_CurrentTrack = PlayTrack(s_PlaylistIndex);
			}

			// Let's sort out the volumes.
			// Next track ramps up based on time.
			float current_track_time_elapsed = Audio::GetTimeElapsed(s_CurrentTrack).toSeconds();
			if (current_track_time_elapsed < FADE_DURATION)
			{
				current_volume = current_track_time_elapsed / FADE_DURATION;
			}
			else if (current_track_time_remaining < FADE_DURATION)
			{
				current_volume = current_track_time_remaining / FADE_DURATION;
			}
			Audio::SetVolume(s_CurrentTrack, current_volume);
		}

		if (s_PreviousTrack)
		{
			float previous_track_time_remaining = Audio::GetTimeRemaining(s_PreviousTrack).toSeconds();
			if (previous_track_time_remaining < 0.0f)
			{
				s_PreviousTrack = nullptr;
			}
			else if (previous_track_time_remaining < FADE_DURATION)
			{
				previous_volume = previous_track_time_remaining / FADE_DURATION;
				Audio::SetVolume(s_PreviousTrack, previous_volume);
			}
		}

		ImGui::Begin("Music monitor");
		if (s_CurrentTrack)
		{ 
			ImGui::Text("Track %d/%d: %s", s_PlaylistIndex + 1, s_Playlist.size(), s_Playlist[s_PlaylistIndex].file.c_str());
			float current_track_time_remaining = Audio::GetTimeRemaining(s_CurrentTrack).toSeconds();
			ImGui::Text("Time remaining: %f", current_track_time_remaining);
			ImGui::Text("Current volume: %f", current_volume);
			ImGui::Text("Previous volume : %f", previous_volume);
			if (ImGui::Button("Previous track"))
			{
				Audio::Stop(s_CurrentTrack);
				Audio::Stop(s_PreviousTrack);
				s_PlaylistIndex = s_PlaylistIndex ? s_PlaylistIndex - 1 : s_Playlist.size() - 1;
				s_CurrentTrack = PlayTrack(s_PlaylistIndex);
			}
			ImGui::SameLine();
			if (ImGui::Button(s_Paused ? "Resume" : "Pause"))
			{
				if (s_Paused)
				{
					ResumePlaying();
				}
				else
				{
					PausePlaying();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Next track"))
			{
				Audio::Stop(s_CurrentTrack);
				Audio::Stop(s_PreviousTrack);
				s_PlaylistIndex = (s_PlaylistIndex + 1) % s_Playlist.size();
				s_CurrentTrack = PlayTrack(s_PlaylistIndex);
			}
		}
		ImGui::End();
	}

	void LoadPlaylist(const std::string& playlist_filename)
	{
		s_Playlist.clear();
		s_PlaylistIndex = 0;
		s_PreviousTrack = s_CurrentTrack = nullptr;
		YAML::Node yaml_node = YAML::LoadFile(playlist_filename);
		for (auto node : yaml_node["playlist"])
		{
			PlaylistTrack track;
			track.file = node["file"].as<std::string>();
			s_Playlist.push_back(track);
		}
	}

	void StartPlaying()
	{
		if (s_PlaylistIndex < s_Playlist.size())
		{
			s_CurrentTrack = PlayTrack(s_PlaylistIndex);
		}
	}

	void PausePlaying()
	{
		Audio::Pause(s_CurrentTrack);
		Audio::Pause(s_PreviousTrack);
		s_Paused = true;
	}

	void ResumePlaying()
	{
		Audio::Resume(s_CurrentTrack);
		Audio::Resume(s_PreviousTrack);
		s_Paused = false;
	}

	void StopPlaying()
	{
		Audio::Stop(s_PreviousTrack);
		Audio::Stop(s_CurrentTrack);
		s_PreviousTrack = s_CurrentTrack = nullptr;
		s_PlaylistIndex = 0;
	}
}