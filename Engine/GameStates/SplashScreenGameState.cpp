#include "Engine/Pch.h"

#include "Engine/GameStates/SplashScreenGameState.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/TextureManager.h"


const float FADE_IN_TIME = 1.0f;
const float FADE_OUT_TIME = 1.0f;

SplashScreenGameState::SplashScreenGameState(const std::string& texture, const Time& duration)
{
	m_Texture = TextureManager::LoadTexture(texture);
	m_Duration = duration + Time::fromSeconds(FADE_IN_TIME + FADE_OUT_TIME);
}

SplashScreenGameState::~SplashScreenGameState()
{
	TextureManager::UnloadTexture(m_Texture);
}

void SplashScreenGameState::OnEnter()
{
	m_Sprite = ScreenSpaceRenderer::AddSprite(0, 0, m_Texture, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

bool SplashScreenGameState::OnUpdate(const Time& frame_time)
{
	m_TimeInState += frame_time;

	float seconds = m_TimeInState.toSeconds();
	if (seconds < FADE_IN_TIME)
	{
		ScreenSpaceRenderer::SetTint(m_Sprite, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (seconds / FADE_IN_TIME));
	}

	if (m_TimeInState > (m_Duration - Time::fromSeconds(FADE_OUT_TIME)))
	{
		ScreenSpaceRenderer::SetTint(m_Sprite, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * ((m_Duration.toSeconds() - seconds) / FADE_OUT_TIME));
	}

	if (Input::GetKeyEvent(GLFW_KEY_SPACE) == Input::PRESSED && m_TimeInState < (m_Duration - Time::fromSeconds(FADE_OUT_TIME)))
	{
		m_TimeInState = m_Duration - Time::fromSeconds(FADE_OUT_TIME);
	}

	return m_TimeInState < m_Duration;
}

void SplashScreenGameState::OnExit()
{
	ScreenSpaceRenderer::RemoveSprite(m_Sprite);
}