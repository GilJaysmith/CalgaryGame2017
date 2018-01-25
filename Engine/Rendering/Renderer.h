#pragma once

#undef CreateWindow

class Camera;
class Renderable;

namespace Renderer
{
	void Initialize();
	
	void Terminate();

	void CreateWindow(unsigned int width, unsigned int height, bool full_screen);

	void DestroyWindow();

	GLFWwindow* GetWindow();

	void ClearWindow();

	void SetActiveCamera(Camera*);

	void RenderScene();

	void RegisterRenderable(Renderable* renderable);
	void UnregisterRenderable(Renderable* renderable);
}
