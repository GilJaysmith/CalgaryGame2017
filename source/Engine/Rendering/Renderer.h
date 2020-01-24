#pragma once

#undef CreateWindow

class Camera;
class Time;
class Renderable;
class Viewport;
class PostProcessEffect;

namespace Renderer
{
	void Initialize();
	
	void Terminate();

	void GetWindowDimensions(unsigned int& width, unsigned int& height);

	GLFWwindow* GetWindow();

	void ClearWindow();

	void SetActiveCamera(Camera*);
	Camera* GetActiveCamera();

	void RenderScene(const Time& frame_time);

	void RegisterRenderable(Renderable* renderable);
	void UnregisterRenderable(Renderable* renderable);

	void RegisterViewport(const Viewport& viewport, const std::string& viewport_name);
	void UnregisterViewport(const std::string& viewport_name);
	Viewport& GetViewport(const std::string& viewport_name);

	void InsertPostProcessEffect(PostProcessEffect* ppe);
}
