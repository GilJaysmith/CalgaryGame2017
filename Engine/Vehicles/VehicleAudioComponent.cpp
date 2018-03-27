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
	for (auto gear_audio : m_GearAudios)
	{
		Audio::Stop(gear_audio);
	}
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

		if (true)
		{
			// Look at revs
			Message_VehicleGetEngineInfo vegi;
			m_Entity->OnMessage(&vegi);
			float revs = vegi.m_EngineRotationSpeed;
			for (int gear_idx = 0; gear_idx < m_GearSounds.size(); ++gear_idx)
			{
				GearSound& gear_sound = m_GearSounds[gear_idx];
				AudioHandle gear_audio = m_GearAudios[gear_idx];
				float desired_volume = 1.0f;
				float desired_pitch = 1.0f;
				if (revs <= gear_sound.low_zero_volume || revs >= gear_sound.high_zero_volume)
				{
					desired_volume = 0.0f;
				}
				if (revs > gear_sound.low_zero_volume && revs < gear_sound.low_full_volume)
				{
					desired_volume = (revs - gear_sound.low_zero_volume) / (gear_sound.low_full_volume - gear_sound.low_zero_volume);
				}
				if (revs > gear_sound.high_full_volume && revs < gear_sound.high_zero_volume)
				{
					desired_volume = 1.0f - ((revs - gear_sound.high_full_volume) / (gear_sound.high_zero_volume - gear_sound.high_full_volume));
				}
				if (revs >= gear_sound.low_zero_volume && revs <= gear_sound.high_zero_volume)
				{
					desired_pitch = 0.5f + 1.5f * (revs - gear_sound.low_zero_volume) / (gear_sound.high_zero_volume - gear_sound.low_zero_volume);
				}
				Audio::SetVolume(gear_audio, desired_volume);
				Audio::SetPitch(gear_audio, desired_pitch);
			}
		}
	}
}

VehicleAudioComponent::VehicleAudioComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	// Initialize sounds.

	// TODO: move to yaml
	m_GearSounds = {
		//{
		//	"low_on.wav",
		//	-100,
		//	0,
		//	200,
		//	300
		//},
		//{
		//	"med_on.wav",
		//	200,
		//	300,
		//	500,
		//	600
		//},
		//{
		//	"high_on.wav",
		//	500,
		//	600,
		//	600,
		//	700
		//}
		{
			"med_on.wav",
			-1,
			0,
			600,
			601
		}
	};


	for (auto gear_sound : m_GearSounds)
	{
		AudioHandle gs = Audio::PlaySound("carengine\\" + gear_sound.sound, true);
		Audio::SetVolume(gs, 0.0f);
		Audio::SetLooping(gs);
		m_GearAudios.push_back(gs);
	}
}