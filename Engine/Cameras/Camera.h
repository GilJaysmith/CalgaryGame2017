#pragma once

class Time;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void Update(const Time& frame_time) = 0;
	glm::mat4 GetViewMatrix() const;
	void SetActive(bool active);

protected:
	glm::mat4 m_ViewMatrix;
	bool m_Active;
};
