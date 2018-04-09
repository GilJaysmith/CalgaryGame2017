#pragma once

class Time;

class PostProcessEffect
{
public:
	PostProcessEffect(const std::string& shader, float duration = FLT_MAX);
	virtual ~PostProcessEffect();

	virtual bool IsComplete() const;
	virtual void Update(const Time& frame_time);
	virtual void RenderTo(GLuint framebuffer) const;

	GLuint GetFrameBuffer() const { return m_FrameBuffer; }
	GLuint GetTextureColourBuffer() const { return m_TextureColourBuffer; }

protected:
	GLuint m_ShaderProgram;
	GLuint m_FrameBuffer;
	GLuint m_TextureColourBuffer;
	GLuint m_QuadVAO;

	float m_Duration;
};