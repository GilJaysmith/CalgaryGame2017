#include "Engine/Pch.h"

#include "Engine/Rendering/TextureManager.h"


namespace TextureManager
{
	void Initialize()
	{

	}

	void Terminate()
	{

	}

	std::map<std::string, GLuint> s_LoadedTextures;

	GLuint LoadTexture(const std::string& filename)
	{
		auto it = s_LoadedTextures.find(filename);
		if (it != s_LoadedTextures.end())
		{
			return it->second;
		}

		GLuint texture_id;
		glGenTextures(1, &texture_id);

		glBindTexture(GL_TEXTURE_2D, texture_id);

		int width, height;
		unsigned char* image = SOIL_load_image(("data/textures/" + filename).c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		SOIL_free_image_data(image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		s_LoadedTextures[filename] = texture_id;

		return texture_id;
	}

	void UnloadTexture(GLuint texture)
	{
		glDeleteTextures(1, &texture);
	}
}
