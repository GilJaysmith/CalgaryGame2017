#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Vehicles/VehicleFollowCamera.h"
#include "Engine/Vehicles/VehicleFollowCameraInputHandler.h"


VehicleFollowCamera::VehicleFollowCamera(Entity* followed, int player_idx)
	: m_FollowedEntity(followed)
{
	m_InputHandler = MemNew(MemoryPool::Vehicles, VehicleFollowCameraInputHandler)(player_idx);

	m_CameraPos = glm::vec3();
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraYaw = glm::pi<float>() / 2.0f;
	m_CameraPitch = -glm::pi<float>() / 5.0f;
}

VehicleFollowCamera::~VehicleFollowCamera()
{
	MemDelete(m_InputHandler);
}

void VehicleFollowCamera::Update(const Time& frame_time)
{
	glm::mat4 entity_transform = m_FollowedEntity->GetTransform();

	glm::mat3 entity_rotation(entity_transform);
	glm::vec3 entity_position(entity_transform[3]);

	glm::vec3 camera_offset(0.0f, 5.0f, -10.0f);
	camera_offset = entity_rotation * camera_offset;
	camera_offset += entity_position;

	m_CameraPos = camera_offset;

	float camera_yaw_speed = 0.2f;
	float camera_pitch_speed = 0.2f;

	m_CameraYaw += camera_yaw_speed * m_InputHandler->Yaw() * frame_time.toSeconds();
	m_CameraPitch += camera_pitch_speed * m_InputHandler->Pitch() * frame_time.toSeconds();

	m_CameraPitch = glm::clamp(m_CameraPitch, -glm::pi<float>() / 2.1f, glm::pi<float>() / 2.1f);

	glm::vec3 front;
	front.x = cos(m_CameraPitch) * cos(m_CameraYaw);
	front.y = sin(m_CameraPitch);
	front.z = cos(m_CameraPitch) * sin(m_CameraYaw);
	m_CameraFront = glm::normalize(front);
	m_CameraFront = entity_rotation * m_CameraFront;

	m_ViewMatrix = glm::lookAt(
		m_CameraPos,
		m_CameraPos + m_CameraFront,
		m_CameraUp);
}
