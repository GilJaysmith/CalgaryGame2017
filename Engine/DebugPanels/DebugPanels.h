#pragma once

class Time;

namespace DebugPanels
{
	void Initialize();

	void Terminate();

	void Update(const Time& last_frame_time);

	void Render();
}
