#pragma once

namespace ScreenSpaceRenderer
{
	void Initialize();

	void Terminate();

	GLuint AddSprite(int x, int y, GLuint texture);
	void RemoveSprite(GLuint sprite);

	void Render();
}
