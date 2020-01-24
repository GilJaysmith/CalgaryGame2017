#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/Cameras/CameraManager.h"
#include "Engine/GameStates/Time.h"


namespace CameraManager
{
	std::vector<Camera*> s_ActiveCameras;

	void Initialize()
	{

	}

	void Terminate()
	{
		for (auto camera : s_ActiveCameras)
		{
			MemDelete(camera);
		}
		s_ActiveCameras.clear();
	}

	void RegisterCamera(Camera* camera)
	{
		s_ActiveCameras.push_back(camera);
	}

	void Update(const Time& frame_time)
	{
		for (auto camera : s_ActiveCameras)
		{
			camera->Update(frame_time);
		}
	}
}
