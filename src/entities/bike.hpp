#pragma once

#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include "game_object.hpp"
#include "physics/physics.h"
#include "render/wall.hpp"
#include "window.hpp"
#include "ui_game.h"


class Bike : public GameObject {
  private:
	int lap = 1;
	int waypoint = 0;
	int place;
	float health = 100;
	int id;
	bool locked = false;

	Window& window;
	Render::Wall wall;
	UiGame* UI;
  protected:
	std::vector<glm::vec3> waypoints; // current waypoints

  public:
	Audio::AudioEngine& stereo;
	AudioInstance* engineAudio = new AudioInstance();
	AudioInstance* gearAudio = new AudioInstance();
	AudioInstance* JumpAudio = new AudioInstance();
	AudioInstance* StrafeAudio = new AudioInstance();
	AudioInstance* WADAudio = new AudioInstance();
	AudioInstance* FRAGAudio = new AudioInstance();
	AudioInstance* chassisAudio = new AudioInstance();

	Bike(Window& window, Render::Render & render, int start_place, Render::Group& group, Audio::AudioEngine& audio,
		Render::MaterialHandle wallMaterialHandle, UiGame* UI)
		: window(window), GameObject(render, group), place(start_place), id(start_place - 1), stereo(audio),
		  wall(render, wallMaterialHandle), UI(UI) {
		FRAGAudio->gain = 1.f;
	};

	int getId() { return id; }

	virtual void update(float deltaTime) override {
		model->setTransform(convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));

		this->wall.append_wall(convertTransform(getBikeTransform(getId())), {0, 0, -4.8}, {0.1, 1});

		spawnWall(deltaTime, getId());
	}

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
	void modifyHealth(int amount) { 
		if (amount > 0) {
			if (health + amount > 100) {
				health = 100;
			} else {
				health += amount;
			}
		} else if (amount < 0) {
			if (health + amount < 0) {
				health = 0;
			} else {
				health += amount;
			}
		}
		std::cout << health << std::endl;
		UI->updateSI(health);
	}

	void wallCollision() {
		modifyHealth(-25);
		chassisAudio->playSound(stereo.buffer[Audio::SOUND_FILE_BIKE_IMPACT_SFX]);

		// std::cout << "current health = " << health << std::endl;
		// play sound?
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
				}
				wall->wallFront = vehicle->getRigidDynamicActor()->getGlobalPose();
				wall->timer = 0.0f;
			}
		} else {
			wall->wallFront.p.x = NULL;
		}
	}
};