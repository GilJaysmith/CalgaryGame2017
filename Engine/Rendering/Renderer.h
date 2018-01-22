#pragma once

#undef CreateWindow

namespace Rendering
{
	void Initialize();
	
	void Terminate();

	void CreateWindow(unsigned int width, unsigned int height, bool full_screen);

	void DestroyWindow();

	GLFWwindow* GetWindow();

	void ClearWindow();
}
