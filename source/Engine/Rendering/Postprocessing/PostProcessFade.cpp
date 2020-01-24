#include "Engine/Pch.h"

#include "Engine/Rendering/Postprocessing/PostProcessFade.h"
#include "Engine/Rendering/ShaderManager.h"


PostProcessFadeOut::PostProcessFadeOut(const glm::vec3& fade_to, float duration)
	: PostProcessEffect("PostProcessFade", duration)
	, m_Tint(fade_to)
	, m_OrigDuration(duration)
{

}

void PostProcessFadeOut::RenderTo(GLuint framebuffer) const
{
	ShaderManager::SetActiveShader(m_ShaderProgram);
	GLuint uniTint = ShaderManager::GetUniformLocation(m_ShaderProgram, "tint");
	glm::vec4 tint(m_Tint, glm::max(m_Duration, 0.0f) / m_OrigDuration);
	glUniform4fv(uniTint, 1, glm::value_ptr(tint));
	PostProcessEffect::RenderTo(framebuffer);
}


PostProcessFadeIn::PostProcessFadeIn(const glm::vec3& fade_to, float duration)
	: PostProcessEffect("PostProcessFade", duration)
	, m_Tint(fade_to)
	, m_OrigDuration(duration)
{

}

void PostProcessFadeIn::RenderTo(GLuint framebuffer) const
{
	ShaderManager::SetActiveShader(m_ShaderProgram);
	GLuint uniTint = ShaderManager::GetUniformLocation(m_ShaderProgram, "tint");
	glm::vec4 tint(m_Tint, glm::min(m_OrigDuration - m_Duration, m_OrigDuration) / m_OrigDuration);
	glUniform4fv(uniTint, 1, glm::value_ptr(tint));
	PostProcessEffect::RenderTo(framebuffer);
}