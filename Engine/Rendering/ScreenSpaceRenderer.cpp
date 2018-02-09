#include "Engine/Pch.h"

#include "Engine/imGUI/imgui.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"

#include <map>

namespace ScreenSpaceRenderer
{
	GLuint s_ScreenSpaceShader;
	GLuint s_ImGuiShader;
	GLuint s_ImGuiTexture;
	GLuint s_VboHandle;
	GLuint s_ElementsHandle;

	void ImGuiRenderFunction(ImDrawData* draw_data)
	{
		float fb_height = 600.0f;
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			glBindBuffer(GL_ARRAY_BUFFER, s_VboHandle);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
	}

	void Initialize()
	{
		s_ScreenSpaceShader = ShaderManager::LoadProgram("ScreenSpaceShader");
		s_ImGuiShader = ShaderManager::LoadProgram("ImGuiShader");
		
		ImGui::CreateContext();
		
		ImGuiIO& io = ImGui::GetIO();
		unsigned int x, y;
		Renderer::GetWindowDimensions(x, y);
		io.DisplaySize.x = static_cast<float>(x);
		io.DisplaySize.y = static_cast<float>(y);
		io.RenderDrawListsFn = ImGuiRenderFunction;

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		glGenTextures(1, &s_ImGuiTexture);
		glBindTexture(GL_TEXTURE_2D, s_ImGuiTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		io.Fonts->SetTexID(reinterpret_cast<void*>(static_cast<intptr_t>(s_ImGuiTexture)));

		glGenBuffers(1, &s_VboHandle);
		glGenBuffers(1, &s_ElementsHandle);
	}

	void Terminate()
	{
		ImGui::DestroyContext();
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
		GLint uniTint = glGetUniformLocation(s_ScreenSpaceShader, "model_tint");
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

		bool p_open;
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Example: Long text display", &p_open);
		ImGui::BulletText("Double-click on title bar to collapse window.");
		ImGui::End();

		ImGui::Render();
	}
}
