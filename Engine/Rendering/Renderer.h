#pragma once

#undef CreateWindow

class Camera;
class Renderable;

namespace Renderer
{
	void Initialize();
	
	void Terminate();

	void GetWindowDimensions(unsigned int& width, unsigned int& height);

	GLFWwindow* GetWindow();

	void ClearWindow();

	void SetActiveCamera(Camera*);

	void RenderScene();

	void RegisterRenderable(Renderable* renderable);
	void UnregisterRenderable(Renderable* renderable);
}
