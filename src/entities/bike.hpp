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
	int collisionCD = 60;

	Window& window;
	Render::Wall wall;
	UiGame* UI;
  protected:
	std::vector<glm::vec3> waypoints; // current waypoints
	int WADCharge = 0;
	bool WADRelease = false;

  public:
	Audio::AudioEngine& stereo;
	AudioInstance* engineAudio = new AudioInstance();
	AudioInstance* gearAudio = new AudioInstance();
	AudioInstance* JumpAudio = new AudioInstance();
	AudioInstance* StrafeAudio = new AudioInstance();
	AudioInstance* WADAudio = new AudioInstance();
	AudioInstance* FRAGAudio = new AudioInstance();
	AudioInstance* FRAGImpactAudio = new AudioInstance();
	AudioInstance* chassisAudio = new AudioInstance();
	AudioInstance* SlipstreamingAudio = new AudioInstance();

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

		if (collisionCD > 0) {
			collisionCD--;
		}

		wallCollision();
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
	void modifyHealth(float amount) { 
		if (amount > 0) {
			if (health + amount > 100) {
				health = 100;
			} else {
				health += amount;
				SlipstreamingAudio->playSound(stereo.buffer[Audio::SOUND_FILE_HEALING_SFX]);
			}
		} else if (amount < 0) {
			if (health + amount < 0) {
				health = 0;
				engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_DESPAWN_SFX]);

				lockBike();
			} else {
				health += amount;
			}
		}
		//std::cout << health << std::endl;
		UI->updateSI(health);
	}

	void wallCollision() {
		if (getId() == 0) {
			if (collision(getId()) && collisionCD <= 0) {
				modifyHealth(-10);
				collisionCD = 60;
				chassisAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BIKE_IMPACT_SFX]);
			}
		}
	}

	void spawnWall(float timestep, int i) {
		//Standard Wall generation
		physx::PxVehicleDrive4W* vehicle = getVehicle(i);
		wallSpawnInfo* wall = getWallInfo(i);

		wall->timer += timestep;

		if (vehicle->computeForwardSpeed() >= 10.0f &&
			vehicle->mDriveDynData.getCurrentGear() != physx::PxVehicleGearsData::eREVERSE) {
			if (wall->timer >= wall->wallTime) {
				if (wall->wallFront.p.x != NULL) {
					wall->wallBack = wall->wallFront;
					wall->wallFront.p = getBikeTransform(i).p - 4.8f * getBikeTransform(i).q.getBasisVector2();
					wall->wallFront.q = getBikeTransform(i).q;

					makeWallSeg(i, wall->wallBack, wall->wallFront);
				}
				wall->wallFront.p = getBikeTransform(i).p - 4.8f * getBikeTransform(i).q.getBasisVector2();
				wall->wallFront.q = getBikeTransform(i).q;
				wall->timer = 0.0f;
			}
		} else {
			wall->wallFront.p.x = NULL;
		}
		//WAD wall generation
		if (WADRelease) {
			physx::PxTransform start = getBikeTransform(getId());
			physx::PxTransform end = getBikeTransform(getId());
			// origin behind the bike's +Z then, with a length based on WADcharge,
			// extend out +/- x-axes of the bike to create the start and end points
			physx::PxVec3 z = -start.q.getBasisVector2();
			physx::PxVec3 x = start.q.getBasisVector0();
			physx::PxVec3 wallCentre = start.p + 3 * z;

			start.q *= physx::PxQuat(0, -1, 0, 0);

			start.p = wallCentre + 0.5 * x * WADCharge;
			end.p = wallCentre + -0.5 * x * WADCharge;
			makeWallSeg(0, start, end);


			WADCharge = 0;
			WADRelease = false;
		}
	}
};