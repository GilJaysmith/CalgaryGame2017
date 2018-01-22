#pragma once

namespace TextureManager
{
	void Initialize();

	void Terminate();

	GLuint LoadTexture(const std::string& filename);
}