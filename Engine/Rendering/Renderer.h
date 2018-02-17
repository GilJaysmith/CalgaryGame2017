#pragma once

#undef CreateWindow

class Camera;
class Renderable;
class Viewport;

namespace Renderer
{
	void Initialize();
	
	void Terminate();

	void GetWindowDimensions(unsigned int& width, unsigned int& height);

	GLFWwindow* GetWindow();

	void ClearWindow();

	void SetActiveCamera(Camera*);
	Camera* GetActiveCamera();

	void RenderScene();

	void RegisterRenderable(Renderable* renderable);
	void UnregisterRenderable(Renderable* renderable);

	void RegisterViewport(const Viewport& viewport, const std::string& viewport_name);
	void UnregisterViewport(const std::string& viewport_name);
	Viewport& GetViewport(const std::string& viewport_name);
}
