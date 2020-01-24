#include "Engine/Pch.h"

#include "Engine/GameStates/SplashScreenGameState.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/TextureManager.h"


const Time FADE_IN_TIME = Time::fromSeconds(1.0f);
const Time FADE_OUT_TIME = Time::fromSeconds(1.0f);

SplashScreenGameState::SplashScreenGameState(const std::string& texture, const Time& duration)
{
	m_Texture = TextureManager::LoadTexture(texture);
	m_Duration = duration + FADE_IN_TIME + FADE_OUT_TIME;
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

	if (m_TimeInState < FADE_IN_TIME)
	{
		ScreenSpaceRenderer::SetTint(m_Sprite, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (m_TimeInState.toSeconds() / FADE_IN_TIME.toSeconds()));
	}

	if (m_TimeInState > m_Duration - FADE_OUT_TIME)
	{
		ScreenSpaceRenderer::SetTint(m_Sprite, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * ((m_Duration - m_TimeInState).toSeconds() / FADE_OUT_TIME.toSeconds()));
	}

	if (Input::IsPressed(GLFW_KEY_SPACE))
	{
		if (m_TimeInState < FADE_IN_TIME)
		{
			m_TimeInState = m_Duration - m_TimeInState;
		}
		else
		{
			m_TimeInState = m_Duration - FADE_OUT_TIME;
		}

	}

	return m_TimeInState < m_Duration;
}

void SplashScreenGameState::OnExit()
{
	ScreenSpaceRenderer::RemoveSprite(m_Sprite);
}