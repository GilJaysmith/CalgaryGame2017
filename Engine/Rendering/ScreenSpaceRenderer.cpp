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
	std::map<GLuint, glm::vec4> s_Tints;

	GLuint AddSprite(int x, int y, GLuint texture, const glm::vec4& tint)
	{
		unsigned int width, height;
		Renderer::GetWindowDimensions(width, height);

		float xf = x / (float)width;
		float yf = 1.0f - y / (float)height;

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
		if (posAttrib > -1)
		{
			glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
			glEnableVertexAttribArray(posAttrib);
		}
		GLint tcAttrib = glGetAttribLocation(s_ScreenSpaceShader, "texcoord");
		if (tcAttrib > -1)
		{
			glVertexAttribPointer(tcAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
			glEnableVertexAttribArray(tcAttrib);
		}

		s_ActiveSprites[vao] = texture;
		s_Tints[vao] = tint;
		return vao;
	}

	void RemoveSprite(GLuint sprite)
	{
		// TODO: release GL resources?
		s_ActiveSprites.erase(sprite);
	}

	void SetTint(GLuint sprite, const glm::vec4& tint)
	{
		s_Tints[sprite] = tint;
	}

	void Render()
	{
		ShaderManager::SetActiveShader(s_ScreenSpaceShader);
		GLint uniTint = glGetUniformLocation(s_ScreenSpaceShader, "tint");
		glDisable(GL_DEPTH_TEST);
		for (auto vao : s_ActiveSprites)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, vao.second);

			glUniform4fv(uniTint, 1, glm::value_ptr(s_Tints[vao.first]));
			glBindVertexArray(vao.first);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glEnable(GL_DEPTH_TEST);
		ShaderManager::SetActiveShader(0);
	}
}
