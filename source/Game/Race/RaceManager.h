#pragma once

#include "Engine/Entities/EntityHandle.h"

class Time;

namespace RaceManager
{
	void Initialize();
	void Terminate();

	void InitializeRace(const std::string& race);
	unsigned int GetNumRacers();
	void RegisterCheckpoint(EntityHandle checkpoint);
	void RegisterCar(EntityHandle car);
	EntityHandle GetNextCheckpoint(EntityHandle car);
	bool CarReachesCheckpoint(EntityHandle car, EntityHandle checkpoint);
	std::vector<EntityHandle> GetCheckpoints();
	std::vector<EntityHandle> GetCars();

	struct CarRaceStatus
	{
		EntityHandle m_Car;
		EntityHandle m_NextCheckpoint;
		unsigned int m_NextCheckpointIdx;
		float m_DistanceToNextCheckpoint;
		bool operator<(const CarRaceStatus& other) const
		{
			return m_NextCheckpointIdx > other.m_NextCheckpointIdx
				|| (m_NextCheckpointIdx == other.m_NextCheckpointIdx && m_DistanceToNextCheckpoint < other.m_DistanceToNextCheckpoint);
		}
	};

	std::vector<CarRaceStatus> GetRaceStatus();
	void TerminateRace();
}
