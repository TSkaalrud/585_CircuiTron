#pragma once

#include "game_object.hpp"
#include "wall_manager.hpp"
#include "physics/physics.h"
#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"

class Bike : public GameObject {
  private:
	int lap = 1;
	int waypoint = 0;
	int place;
	float health = 100;
	int id;
	bool locked = false;

	WallManager* wm;

  public:
	Audio::AudioEngine& stereo;
	AudioInstance* engineAudio = new AudioInstance();
	AudioInstance* gearAudio = new AudioInstance();
	AudioInstance* JumpAudio = new AudioInstance();
	AudioInstance* StrafeAudio = new AudioInstance();
	AudioInstance* WADAudio = new AudioInstance();
	AudioInstance* FRAGAudio = new AudioInstance();
	AudioInstance* chassisAudio = new AudioInstance();


	Bike(Render::Render& render, int start_place, Render::Group& group, Audio::AudioEngine& audio, WallManager* wm)
		: GameObject(render, group), place(start_place), id(start_place-1), stereo(audio), wm(wm)
	{
		FRAGAudio->gain = 1.f;
	};

	int getId() { return id; }

	void lockBike() { 
		locked = true;
		bikeReleaseAll(id);
		engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_DESPAWN_SFX]);
	}
	void unlockBike() { locked = false; }
	bool getLocked() { return locked; }

	void setPlace(int n) { place = n; }
	int getPlace() { return place; }

	void addLap() { lap++; }
	int getLap() { return lap; }

	void addWaypoint() { waypoint++; }
	void resetWaypoint() { waypoint = 0; }
	int getWaypoint() { return waypoint; }

	int getHealth() { return health; }
	void modifyHealth(int amount) { health += amount; }

	void wallCollision() { 
		modifyHealth(-25);
		chassisAudio->playSound(stereo.buffer[Audio::SOUND_FILE_BIKE_IMPACT_SFX]);

		//std::cout << "current health = " << health << std::endl;
		//play sound?
	}
	
	void spawnWall(float timestep, int i) {
		physx::PxVehicleDrive4W* vehicle = getVehicle(i);
		wallSpawnInfo* wall = getWallInfo(i);

		wall->timer += timestep;

		if (vehicle->computeForwardSpeed() >= 10.0f &&
			vehicle->mDriveDynData.getCurrentGear() != physx::PxVehicleGearsData::eREVERSE) {
			if (wall->timer >= wall->wallTime) {
				if (wall->wallFront.p.x != NULL) {
					wall->wallBack = wall->wallFront;
					wall->wallFront = vehicle->getRigidDynamicActor()->getGlobalPose();

					makeWallSeg(i, wall->wallBack, wall->wallFront);
					wm->addWall();
				}
				wall->wallFront = vehicle->getRigidDynamicActor()->getGlobalPose();
				wall->timer = 0.0f;
			}
		} else {
			wall->wallFront.p.x = NULL;
		}
	}
};