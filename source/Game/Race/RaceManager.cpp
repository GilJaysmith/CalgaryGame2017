#include "Game/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/Physics/PhysicsUtils.h"
#include "Game/Race/RaceManager.h"

namespace RaceManager
{
	void Initialize()
	{
	}

	void Terminate()
	{
	}

	std::vector<EntityHandle> s_Checkpoints;
	std::vector<EntityHandle> s_Cars;
	std::map<EntityHandle, std::vector<EntityHandle>> s_CarCheckpoints;

	std::vector<glm::mat4> s_CarStartPositions;

	void InitializeRace(const std::string& race)
	{
		s_Cars.clear();
		s_Checkpoints.clear();
		s_CarCheckpoints.clear();
		s_CarStartPositions.clear();

		YAML::Node node = YAML::LoadFile("Data\\Races\\" + race + ".yaml");
		for (const YAML::Node& car_node : node["cars"])
		{
			std::vector<float> p = car_node["position"].as<std::vector<float>>();
			std::vector<float> f = car_node["facing"].as<std::vector<float>>();
			glm::vec3 pos = glm::vec3(p[0], p[1], p[2]);
			glm::vec3 facing = glm::vec3(f[0], f[1], f[2]);
			glm::mat4 rot = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), facing, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 start_position = glm::translate(glm::mat4(), pos) * rot;
			s_CarStartPositions.push_back(start_position);
		}
		for (const YAML::Node& checkpoint_node : node["checkpoints"])
		{
			std::vector<float> pos = checkpoint_node["position"].as<std::vector<float>>();
			Entity* checkpoint = Entity::CreateEntity("checkpoint", glm::translate(glm::mat4(), glm::vec3(pos[0], pos[1], pos[2])));
			RegisterCheckpoint(checkpoint);
		}
	}

	unsigned int GetNumRacers()
	{
		return static_cast<unsigned int>(s_CarStartPositions.size());
	}

	void RegisterCheckpoint(EntityHandle checkpoint)
	{
		s_Checkpoints.push_back(checkpoint);
	}

	void RegisterCar(EntityHandle car)
	{
		assert(s_Cars.size() < s_CarStartPositions.size());
		car->SetTransform(s_CarStartPositions[s_Cars.size()], true);
		s_Cars.push_back(car);
	}

	EntityHandle GetNextCheckpoint(EntityHandle car)
	{
		std::vector<EntityHandle>& car_checkpoints = s_CarCheckpoints[car];
		unsigned int next_checkpoint_idx = static_cast<unsigned int>(car_checkpoints.size());
		if (next_checkpoint_idx == s_Checkpoints.size())
		{
			return EntityHandle();
		}
		return s_Checkpoints[next_checkpoint_idx];
	}

	bool CarReachesCheckpoint(EntityHandle car, EntityHandle checkpoint)
	{
		// How many checkpoints has the car reached so far?
		std::vector<EntityHandle>& car_checkpoints = s_CarCheckpoints[car];
		unsigned int next_checkpoint_idx = static_cast<unsigned int>(car_checkpoints.size());
		if (next_checkpoint_idx < s_Checkpoints.size() && checkpoint.GetEntity() == s_Checkpoints[next_checkpoint_idx].GetEntity())
		{
			std::stringstream str;
			str << "Car " << car.GetEntity()->GetType() << " reaches checkpoint " << next_checkpoint_idx;
			Logging::Log("Race", str.str());
			car_checkpoints.push_back(s_Checkpoints[next_checkpoint_idx]);
			return true;
		}
		return false;
	}

	std::vector<EntityHandle> GetCheckpoints()
	{
		return s_Checkpoints;
	}

	std::vector<EntityHandle> GetCars()
	{
		return s_Cars;
	}

	std::vector<CarRaceStatus> GetRaceStatus()
	{
		std::vector<CarRaceStatus> race_status;
		for (auto car : s_Cars)
		{
			if (car.IsValid())
			{
				std::vector<EntityHandle>& car_checkpoints = s_CarCheckpoints[car];
				CarRaceStatus car_race_status;
				car_race_status.m_Car = car;
				car_race_status.m_NextCheckpointIdx = static_cast<unsigned int>(car_checkpoints.size());
				car_race_status.m_NextCheckpoint = car_checkpoints.size() == s_Checkpoints.size() ? EntityHandle() : s_Checkpoints[car_checkpoints.size()];
				car_race_status.m_DistanceToNextCheckpoint = car_checkpoints.size() == s_Checkpoints.size() ? 0.0f : glm::length(car_race_status.m_NextCheckpoint->GetTransform()[3] - car->GetTransform()[3]);
				race_status.push_back(car_race_status);
			}
		}
		std::sort(race_status.begin(), race_status.end());
		return race_status;
	}

	void TerminateRace()
	{
		s_Checkpoints.clear();
		s_Cars.clear();
	}
}
