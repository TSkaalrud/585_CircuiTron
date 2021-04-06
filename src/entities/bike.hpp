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

  protected:
	//Game
	bool& menuActive;
	Window& window;
	UiGame* UI;
	Render::Wall wall;

	//Race
	int place, lap = 1, waypoint = 0, currentWaypoint = 0, nextWaypoint = 1;
	std::vector<glm::vec3> waypoints; // current waypoints

	//Bike
	int currentGear = 2;
	float health = 100;
	int id;
	bool locked = false;

	//Abilities
	int BoostCD = 0, collisionCD = 60, FRAGCD = 0, resettingCD = 0, SlipstreamCD = 30, StrafeCD = 0;
	bool resetting = false, Slipstreaming = false, WADRelease = false;
	int Slipstreams = 0, WADCharge = 0;

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
		Render::MaterialHandle wallMaterialHandle, UiGame* UI, bool& menuActive)
		: window(window), GameObject(render, group), place(start_place), id(start_place - 1), stereo(audio),
		  wall(render, wallMaterialHandle), UI(UI), menuActive(menuActive) {
		FRAGAudio->gain = 1.f;
		engineAudio->loop = true;
		gearAudio->loop = false;
		FRAGAudio->loop = false;
	};

	int getId() { return id; }

	virtual void update(float deltaTime) override {

		if (!menuActive) {

			model->setTransform(
				convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));

			this->wall.append_wall(convertTransform(getBikeTransform(getId())), {0, 0, -4.8}, {0.1, 1});

			spawnWall(deltaTime, getId());
			if (!getLocked()) {
				// reduce CD's
				if (BoostCD > 0) {
					BoostCD--;
				}
				if (StrafeCD > 0) {
					StrafeCD--;
				}
				if (FRAGCD > 0) {
					FRAGCD--;
				}
				if (collisionCD > 0) {
					collisionCD--;
				}
				if (resettingCD > 0) {
					resettingCD--;
				}
				// off track auto reset
				if (getBikeTransform(getId()).p.y < 0) {
					resetBike();
				}
				slipstreaming();
				wallCollision();
				engineSounds();
			}
		} else {
			//pause sounds
		}
	}

	void engineSounds() {
		// Audio
		// Rev check
		int gear = getBikeGear(getId());
		if (currentGear != gear) {
			if (gear == 0) { // reset audio levels when reversing (first gear is used during gear shifts)
				engineAudio->changePitch(1);
				gearAudio->changePitch(1);
				engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_IDLE_WUB_SFX]);
				gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_DOWN2_SFX]);

			} else {
				float delta = 0.1f;
				if (currentGear < gear && gear != 1) {
					delta = delta * (gear - currentGear); // increase pitch (accelerating) (max 2.0)
					engineAudio->changePitch(1 + delta);
					gearAudio->changePitch(1 + delta);

					gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_UP2_SFX]);
				} else if (currentGear > gear && gear != 1) {
					delta = delta * (currentGear - gear); // decrease pitch (decelerating) (min 0.5)
					engineAudio->changePitch(1 - delta);
					gearAudio->changePitch(1 - delta);
					gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_DOWN2_SFX]);
				}
			}
		}
		// Play pitch corrected revs (on loop)
		engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_REV_STEADY_SFX]);
		currentGear = gear;
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
		if (getId() == 0) {
			UI->updateSI(health);
		}
	}

	//Bike collisions with walls
	void wallCollision() {
		if (getId() == 0) {
			if (collision(getId()) && collisionCD <= 0) {
				modifyHealth(-10);
				collisionCD = 60;
				chassisAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BIKE_IMPACT_SFX]);
			}
		}
	}

	//Wall generation for driving and WAD 
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
			physx::PxVec3 wallCentre = start.p + 4.8 * z;

			start.q *= physx::PxQuat(0, -1, 0, 0);

			start.p = wallCentre + 0.5 * x * WADCharge;
			end.p = wallCentre + -0.5 * x * WADCharge;
			makeWallSeg(0, start, end);

			WADAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_SIZZLE_SFX]);
			WADAudio->loop = false;
			
			modifyHealth(-WADCharge / 5);
			WADCharge = 0;
			WADRelease = false;
		}
		if (WADCharge > 0) {
			WADAudio->loop = true;
			WADAudio->playSound(stereo.buffer[Audio::SOUND_FILE_WAD_SFX]);
		}
	}

	//Reset and orient the bike on the track.
	void resetBike() {
		physx::PxTransform resetLocation = getBikeTransform(getId());
		int waypoint = 0;
		int waypointOffset = 4;
		// waypointOffset is used to manage the fact that we hit waypoints within a certain radius of us.
		if (currentWaypoint < waypointOffset) {
			waypoint = waypoints.size() - (waypointOffset - currentWaypoint);
		} else {
			waypoint = currentWaypoint - waypointOffset;
		}

		resetLocation.p.x = waypoints[waypoint].x;
		resetLocation.p.y = waypoints[waypoint].y + 5; // drop me in to avoid clipping
		resetLocation.p.z = waypoints[waypoint].z;
		float rads;
		physx::PxVec3 axis;
		resetLocation.q.toRadiansAndUnitAxis(rads, axis);
		resetLocation.q.x = 0;
		resetLocation.q.y = sin(rads / 2);
		resetLocation.q.z = 0;
		resetLocation.q.w = cos(rads / 2);
		resetBikePos(getId(), resetLocation);
		modifyHealth(-33);
		resettingCD += 60;
	}

	//Hitscan bike cannon, the FRAG
	bool fragFire(int bike) {
		auto wallPointer = fragRay(bike, 100);

		if (wallPointer != NULL) {
			// Possibly put wall deletion here
			FRAGImpactAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT2_SFX]);
			return true;
		} else {
			// play FRAG fired (but no hit) sound
			FRAGAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT_SFX]);
			return false;
		}
	}

	int slipstreams(int bike) {
		int slipstreamCount = 0;

		if (slipstreamRay(bike, 2, 5)) {
			slipstreamCount++;
		}

		if (slipstreamRay(bike, 3, 5)) {
			slipstreamCount++;
		}

		/*
		physx::PxRaycastBuffer* leftRay = castRay(bike, 2, 10);
		physx::PxRaycastBuffer* rightRay = castRay(bike, 3, 10);

		if (leftRay->nbTouches > 0) {
			const char* leftName = leftRay->touches[0].actor->getName();
			std::cout << leftName << std::endl;
			if (std::strcmp(leftName, "wall") == 0) {
				slipstreamCount++;
			}
		}

		if (rightRay->nbTouches > 0) {
			const char* rightName = rightRay->touches[0].actor->getName();
			std::cout << rightName << std::endl;
			if (std::strcmp(rightName, "wall") == 0) {
				slipstreamCount++;
			}
		}
		*/

		return slipstreamCount;
	}

	void slipstreaming() {
		// Slipstreaming
		Slipstreams = slipstreams(getId());
		if (Slipstreams > 0) {
			if (SlipstreamCD > 0) {
				SlipstreamCD--;
				Slipstreaming = false;
			} else {
				modifyHealth(0.25 * Slipstreams);
				Slipstreaming = true;
				/*
				physx::PxVec3 forward = getBikeTransform(getId()).q.getBasisVector2() * 50;
				getVehicle(getId())->getRigidDynamicActor()->addForce(
					forward, physx::PxForceMode::eACCELERATION);
				*/
			}
		} else if (SlipstreamCD < 30) {
			SlipstreamCD++;
			Slipstreaming = false;
		}
	}
};