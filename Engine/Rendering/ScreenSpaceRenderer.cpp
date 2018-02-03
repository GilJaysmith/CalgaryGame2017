#include "Engine/Pch.h"

#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"

#include <map>

namespace ScreenSpaceRenderer
{
	GLuint s_ScreenSpaceShader;

	void Initialize()
	{
		s_ScreenSpaceShader = ShaderManager::LoadProgram("ScreenSpaceShader");
	}

	void Terminate()
	{

	}

	std::map<GLuint, GLuint> s_ActiveSprites;

	GLuint AddSprite(int x, int y, GLuint texture)
	{
		unsigned int width, height;
		Renderer::GetWindowDimensions(width, height);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float vert_data[] = 
		{
			-1.0f, 1.0f, 0.0f, 0.0f,	// top left
			1.0f, 1.0f, 1.0f, 0.0f,	    // top right
			-1.0f, -1.0f, 0.0f, 1.0f,	// bottom left
			1.0f, 1.0f, 1.0f, 0.0f,	// top right
			1.0f, -1.0f, 1.0f, 1.0f,	// bottom right
			-1.0f, -1.0f, 0.0f, 1.0f	// bottom left
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);

		GLint posAttrib = glGetAttribLocation(s_ScreenSpaceShader, "position");
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(posAttrib);
		GLint tcAttrib = glGetAttribLocation(s_ScreenSpaceShader, "texcoord");
		glVertexAttribPointer(tcAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(tcAttrib);

		//glBindVertexArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		s_ActiveSprites[vao] = texture;
		return vao;
	}

	void RemoveSprite(GLuint sprite)
	{
		// TODO: release releases
		s_ActiveSprites.erase(sprite);
	}

	void Render()
	{
		glUseProgram(s_ScreenSpaceShader);
		glDisable(GL_DEPTH);
		for (auto vao : s_ActiveSprites)
		{
			glActiveTexture(0);
			glBindTexture(GL_TEXTURE_2D, vao.second);

			glBindVertexArray(vao.first);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
		glEnable(GL_DEPTH);
	}
}
