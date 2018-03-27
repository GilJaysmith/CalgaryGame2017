#include "Engine/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Vehicles/VehicleAudioComponent.h"
#include "Engine/Vehicles/VehicleMessages.h"


Component* VehicleAudioComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Entities, VehicleAudioComponent)(owner, properties);
}

VehicleAudioComponent::~VehicleAudioComponent()
{
	for (auto engine_sound : m_EngineSounds)
	{
		Audio::Stop(engine_sound);
	}
	Audio::Stop(m_SkidSound);
}

bool VehicleAudioComponent::OnMessage(Message* message)
{
	return false;
}

void VehicleAudioComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	if (update_pass == UpdatePass::AfterPhysics)
	{
		// Audio.
		// Max engine rotation speed is 600.
		// Let's assign the wavs as follows:
		// low: full volume 0-100, fall off by 200
		// med: ramp up 100-200, full volume to 400, fall off by 500
		// high: ramp up 400-500, full volume to 600
		// At the bottom of the range, pitch = 0.5
		// At the top of the range, pitch = 2.0


		// Get car position.
		glm::vec3 car_position = m_Entity->GetTransform()[3];

		// Get vehicle info.
		Message_VehicleGetDynamicInfo vgdi;
		m_Entity->OnMessage(&vgdi);

		ImGui::SetNextWindowSizeConstraints(ImVec2(400, 100), ImVec2(800, 600));
		ImGui::Begin("Car audio debug");

		// Engine sounds.
		if (true)
		{
			float revs = vgdi.m_EngineRotationSpeed;
			ImGui::Text("Revs: %f", revs);
			for (int gear_idx = 0; gear_idx < m_EngineSoundDefs.size(); ++gear_idx)
			{
				EngineSoundDef& engine_sound_def = m_EngineSoundDefs[gear_idx];
				AudioHandle engine_sound = m_EngineSounds[gear_idx];

				float desired_volume = 1.0f;
				float desired_pitch = 1.0f;
				if (revs <= engine_sound_def.low_zero_volume || revs >= engine_sound_def.high_zero_volume)
				{
					desired_volume = 0.0f;
				}
				if (revs > engine_sound_def.low_zero_volume && revs < engine_sound_def.low_full_volume)
				{
					desired_volume = (revs - engine_sound_def.low_zero_volume) / (engine_sound_def.low_full_volume - engine_sound_def.low_zero_volume);
				}
				if (revs > engine_sound_def.high_full_volume && revs < engine_sound_def.high_zero_volume)
				{
					desired_volume = 1.0f - ((revs - engine_sound_def.high_full_volume) / (engine_sound_def.high_zero_volume - engine_sound_def.high_full_volume));
				}
				if (revs >= engine_sound_def.low_zero_volume && revs <= engine_sound_def.high_zero_volume)
				{
					desired_pitch = 0.5f + 1.5f * (revs - engine_sound_def.low_zero_volume) / (engine_sound_def.high_zero_volume - engine_sound_def.low_zero_volume);
				}

				// A little random variation in the pitch, to avoid warping effects when multiple cars are close together.
				float MAX_VARIATION = 0.0f;
				float variation = (rand() / (float)RAND_MAX) * MAX_VARIATION - (rand() / (float)RAND_MAX) * MAX_VARIATION;
				desired_pitch += variation;

				Audio::SetVolume(engine_sound, desired_volume);
				Audio::SetPitch(engine_sound, desired_pitch);
				Audio::SetPosition(engine_sound, car_position);
			}
		}

		// Skidding.
		// This is where our actual velocity does not match our facing.
		float skid_divergence = 0.0f;
		if (true)
		{
			if (!vgdi.m_InAir && glm::length(vgdi.m_LinearVelocity) >= 0.01f)
			{
				glm::vec3 car_facing = glm::normalize(m_Entity->GetTransform()[2]);
				glm::vec3 car_velocity = glm::normalize(vgdi.m_LinearVelocity);
				skid_divergence = 1.0f - abs(glm::dot(car_facing, car_velocity));

				const float MIN_SKID_DIVERGENCE = 0.0f;
				float skid_volume = glm::clamp(skid_divergence - MIN_SKID_DIVERGENCE, 0.0f, 1.0f);
				float car_velocity_mag = glm::length(vgdi.m_LinearVelocity);
				const float MAG_SCALE = 1.0f;
				float speed_mag = car_velocity_mag / MAG_SCALE;
				skid_volume = glm::clamp(skid_volume * speed_mag, 0.0f, 5.0f);
				Audio::SetVolume(m_SkidSound, skid_volume);
				Audio::SetPosition(m_SkidSound, car_position);

				ImGui::Text("Car velocity: (%f, %f, %f)", vgdi.m_LinearVelocity.x, vgdi.m_LinearVelocity.y, vgdi.m_LinearVelocity.z);
				ImGui::Text("Skid divergence: %f", skid_divergence);
				ImGui::Text("Final volume: %f", skid_volume);
				ImGui::Text("Speed mag: %f", speed_mag);
			}
			else
			{
				Audio::SetVolume(m_SkidSound, 0.0f);
			}
		}

		ImGui::End();
	}
}

namespace YAML
{
	template <>
	struct convert<VehicleAudioComponent::EngineSoundDef> {
		static Node encode(const VehicleAudioComponent::EngineSoundDef& rhs) { 
			return Node(); 
		}

		static bool decode(const Node& node, VehicleAudioComponent::EngineSoundDef& rhs)
		{
			if (node.IsNull()) return false;
			rhs.sound = node["sound"].as<std::string>();
			rhs.low_zero_volume = node["low_zero_volume"].as<float>();
			rhs.low_full_volume = node["low_full_volume"].as<float>();
			rhs.high_full_volume = node["high_full_volume"].as<float>();
			rhs.high_zero_volume = node["high_zero_volume"].as<float>();
			return true;
		}
	};
}

VehicleAudioComponent::VehicleAudioComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_SkidSound(nullptr)
{
	// Initialize sounds.
	if (properties["engine"])
	{
		m_EngineSoundDefs = properties["engine"].as<std::vector<EngineSoundDef>>();
	}
	for (auto engine_sound : m_EngineSoundDefs)
	{
		AudioHandle handle = Audio::PlaySound("car\\" + engine_sound.sound, glm::vec3(), true);
		Audio::SetVolume(handle, 0.0f);
		Audio::SetLooping(handle);
		m_EngineSounds.push_back(handle);
	}
	if (properties["skid"])
	{
		std::string skid_sound = properties["skid"].as<std::string>();
		AudioHandle handle = Audio::PlaySound("car\\" + skid_sound, glm::vec3(), true);
		Audio::SetVolume(handle, 0.0f);
		Audio::SetLooping(handle);
		m_SkidSound = handle;
	}
}