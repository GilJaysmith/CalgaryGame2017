#pragma once

#include "Engine/Rendering/Postprocessing/PostProcessEffect.h"


class PostProcessFadeOut : public PostProcessEffect
{
public:
	PostProcessFadeOut(const glm::vec3& fade_to, float duration);

	void RenderTo(GLuint framebuffer) const;

protected:
	glm::vec3 m_Tint;
	float m_OrigDuration;
};


class PostProcessFadeIn : public PostProcessEffect
{
public:
	PostProcessFadeIn(const glm::vec3& fade_to, float duration);

	void RenderTo(GLuint framebuffer) const;

protected:
	glm::vec3 m_Tint;
	float m_OrigDuration;
};
