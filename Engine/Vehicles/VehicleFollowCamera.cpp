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
	m_CameraPitch = -glm::pi<float>() / 9.0f;
}

VehicleFollowCamera::~VehicleFollowCamera()
{
	MemDelete(m_InputHandler);
}

#include "sdks/glm/gtx/euler_angles.hpp"

void VehicleFollowCamera::Update(const Time& frame_time)
{
	glm::mat4 entity_transform = m_FollowedEntity->GetTransform();

	glm::vec3 entity_position(entity_transform[3]);
	glm::mat3 entity_rotation(entity_transform);

	glm::vec3 vec(0.0f, 0.0f, -1.0f);
	vec = entity_rotation * vec;
	float rot_y = atan2(vec.z, vec.x);

	ImGui::Begin("Car camera debug");
	ImGui::Text("Rot y %f", rot_y * 180.0f / glm::pi<float>());
	ImGui::End();

	// Make a matrix for the rotation.
	glm::mat3 rot_around_y = glm::rotate(glm::mat4(), -rot_y, glm::vec3(0.0f, 1.0f, 0.0f));

	float camera_yaw_speed = 0.2f;
	float camera_pitch_speed = 0.2f;
	m_CameraYaw += camera_yaw_speed * m_InputHandler->Yaw() * frame_time.toSeconds();
	m_CameraPitch += camera_pitch_speed * m_InputHandler->Pitch() * frame_time.toSeconds();
	m_CameraPitch = glm::clamp(m_CameraPitch, -glm::pi<float>() / 2.1f, glm::pi<float>() / 2.1f);

	glm::mat4 yaw_rot = glm::rotate(glm::mat4(), m_CameraYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 pitch_rot = glm::rotate(glm::mat4(), m_CameraPitch, glm::vec3(1.0f, 0.0f, 0.0f));

	m_CameraFront = /*yaw_rot * pitch_rot * */ rot_around_y * glm::vec3(0.0f, 0.0f, 1.0f);

	m_CameraPos = entity_position - m_CameraFront * 6.0f;

	m_ViewMatrix = glm::lookAt(
		m_CameraPos,
		m_CameraPos + m_CameraFront,
		m_CameraUp);
}
