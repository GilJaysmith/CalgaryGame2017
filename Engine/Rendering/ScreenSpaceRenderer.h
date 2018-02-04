#pragma once

namespace ScreenSpaceRenderer
{
	void Initialize();

	void Terminate();

	GLuint AddSprite(int x, int y, GLuint texture, const glm::vec4& tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	void RemoveSprite(GLuint sprite);
	
	void SetTint(GLuint sprite, const glm::vec4& tint);

	void Render();
}
