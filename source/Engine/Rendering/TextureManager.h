#pragma once

namespace TextureManager
{
	void Initialize();

	void Terminate();

	GLuint LoadTexture(const std::string& filename);
	GLuint LoadCubemap(const std::string& filename);
	void UnloadTexture(GLuint texture);
}