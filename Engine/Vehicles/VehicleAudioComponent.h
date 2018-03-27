#pragma once

#include "Engine/Entities/Component.h"
#include "Engine/Entities/UpdatePass.h"

class VehicleAudioComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~VehicleAudioComponent();

	virtual bool OnMessage(Message*) override;
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

	struct GearSound
	{
		std::string sound;
		float low_zero_volume;
		float low_full_volume;
		float high_full_volume;
		float high_zero_volume;
	};

protected:
	VehicleAudioComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) {}

	std::vector<GearSound> m_GearSounds;
	std::vector<AudioHandle> m_GearAudios;
};