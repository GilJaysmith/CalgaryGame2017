#pragma once

class Camera;
class Time;

namespace CameraManager
{
	void Initialize();
	void Terminate();

	void RegisterCamera(Camera* camera);
	void Update(const Time& frame_time);
}
