#include "Engine/Pch.h"

#include "Engine/GameStates/Time.h"
#include "Engine/Rendering/Postprocessing/PostProcessEffect.h"
#include "Engine/Rendering//Renderer.h"
#include "Engine/Rendering/ShaderManager.h"


PostProcessEffect::PostProcessEffect(const std::string& shader, float duration)
	: m_Duration(duration)
{
	m_ShaderProgram = ShaderManager::LoadProgram(shader);

	// Set up a framebuffer.
	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	// generate texture
	glGenTextures(1, &m_TextureColourBuffer);
	glBindTexture(GL_TEXTURE_2D, m_TextureColourBuffer);
	unsigned int width, height;
	Renderer::GetWindowDimensions(width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColourBuffer, 0);
	// Set up a renderbuffer
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach it to currently bound framebuffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// check framebuffer is complete
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	// restore default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quad_vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &m_QuadVAO);
	glBindVertexArray(m_QuadVAO);
	GLuint quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(m_ShaderProgram, "position");
	GLint texAttrib = glGetAttribLocation(m_ShaderProgram, "texcoord");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(texAttrib);
}

PostProcessEffect::~PostProcessEffect()
{
}

bool PostProcessEffect::IsComplete() const
{
	return m_Duration <= 0.0f;
}

void PostProcessEffect::Update(const Time& frame_time)
{
	if (m_Duration != FLT_MAX)
	{
		m_Duration -= frame_time.toSeconds();
	}
}

void PostProcessEffect::RenderTo(GLuint framebuffer) const
{
	ShaderManager::SetActiveShader(m_ShaderProgram);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(m_QuadVAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureColourBuffer);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}