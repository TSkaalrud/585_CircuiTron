#pragma once

#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include "game_object.hpp"
#include "physics/physics.h"
#include "render/wall.hpp"
#include "ui_game.h"
#include "wall_spawner.hpp"
#include "window.hpp"

class Bike : public GameObject {
  protected:
	//Game
	bool& menuActive;
	Window& window;
	UiGame* UI;

	//Race
	int place, lap = 1, waypoint = 0, currentWaypoint = 0, nextWaypoint = 1;
	std::vector<glm::vec3> waypoints; // current waypoints

	//Bike
	int currentGear = 2;
	float health = 100;
	int id;
	bool locked = false;

	//Abilities
	int BoostCD = 60, collisionCD = 45, FRAGCD = 30, resettingCD = 150, SlipstreamCD = 30, StrafeCD = 60;
	bool resetting = false, Slipstreaming = false, WADRelease = false;
	int Slipstreams = 0, WADCharge = 0;
	bool BoostOn = false, strafeOn = false;

	WallSpawner wallSpawner;

	const float fragSize = 0.1f;
	int fragCount = 5;
	int fragCounter = 0;
	static Render::MeshHandle fragMesh;
	Render::InstanceHandle fragIns;
	Render::MaterialHandle fragMat;

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
		  wallSpawner(render, wallMaterialHandle), UI(UI), menuActive(menuActive) {

		gearAudio->changeLoop(false);
		FRAGAudio->changeLoop(false);
		JumpAudio->changeLoop(false);
		StrafeAudio->changeLoop(false);
		WADAudio->changeLoop(false);
		FRAGImpactAudio->changeLoop(false);
		chassisAudio->changeLoop(false);
		{
			fragMat = render.create_pbr_material({.albedoFactor = {0, 0, 0, 1}, .emissiveFactor = {1, 0.8, 0}});

			if (fragMesh == -1) {

				glm::vec3 points[] = {{-fragSize, -fragSize, 0.0}, {-fragSize, fragSize, 0.0},
									  {fragSize, fragSize, 0.0},   {fragSize, -fragSize, 0.0},
									  {-fragSize, -fragSize, 1.0}, {-fragSize, fragSize, 1.0},
									  {fragSize, fragSize, 1.0},   {fragSize, -fragSize, 1.0}};
				glm::vec3 normals[] = {{-1, 0, 0}, {0, 1, 0}, {1, 0, 0}, {0, -1, 0}};
				fragMesh = render.create_mesh(Render::MeshDef{
					.verticies = {{
						{points[0], normals[0]},
						{points[1], normals[0]},
						{points[1], normals[1]},
						{points[2], normals[1]},
						{points[2], normals[2]},
						{points[3], normals[2]},
						{points[3], normals[3]},
						{points[0], normals[3]},
						{points[0 + 4], normals[0]},
						{points[1 + 4], normals[0]},
						{points[1 + 4], normals[1]},
						{points[2 + 4], normals[1]},
						{points[2 + 4], normals[2]},
						{points[3 + 4], normals[2]},
						{points[3 + 4], normals[3]},
						{points[0 + 4], normals[3]},
					}},
					.indicies = {
						0 + 0, 8 + 0, 1 + 0, 1 + 0, 8 + 0, 9 + 0, 0 + 2, 8 + 2, 1 + 2, 1 + 2, 8 + 2, 9 + 2,
						0 + 4, 8 + 4, 1 + 4, 1 + 4, 8 + 4, 9 + 4, 0 + 6, 8 + 6, 1 + 6, 1 + 6, 8 + 6, 9 + 6,
					}});
			}
			fragIns = render.create_instance(fragMesh, fragMat);
		}
	};

	~Bike() { 
		engineAudio->~AudioInstance();
		gearAudio->~AudioInstance();
		engineAudio->~AudioInstance();
		JumpAudio->~AudioInstance();
		StrafeAudio->~AudioInstance();
		WADAudio->~AudioInstance();
		FRAGAudio->~AudioInstance();
		FRAGImpactAudio->~AudioInstance();
		chassisAudio->~AudioInstance();
		SlipstreamingAudio->~AudioInstance();

	}

	int getId() { return id; }

	virtual void update(float deltaTime) override {
		if (fragCounter-- == 0) {
			render.instance_set_material(fragIns, -1);
		}

		if (getPhysicsActive()) {
			model->setTransform(
				convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
			if (!menuActive) {

				if (!getLocked()) {
					{
						physx::PxVehicleDrive4W* vehicle = getVehicle(getId());
						wallSpawner.shouldSpawnWall = vehicle->computeForwardSpeed() >= 10.0f &&
							vehicle->mDriveDynData.getCurrentGear() != physx::PxVehicleGearsData::eREVERSE;
					}
					if (WADRelease) {
						wallSpawner.spawnWad.emplace(1 + 0.05 * WADCharge);

						WADAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_SIZZLE_SFX]);
						WADAudio->loop = false;

						modifyHealth(-5);
						WADCharge = 0;
						WADRelease = false;
					}
					if (WADCharge > 0) {
						WADAudio->loop = true;
						WADAudio->playSound(stereo.buffer[Audio::SOUND_FILE_WAD_SFX]);
					}

					wallSpawner.spawnWalls(deltaTime, getId());

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
				// pause sounds
				engineAudio->pauseSound();
				gearAudio->pauseSound();
				JumpAudio->pauseSound();
				StrafeAudio->pauseSound();
				WADAudio->pauseSound();
				FRAGAudio->pauseSound();
				FRAGImpactAudio->pauseSound();
				chassisAudio->pauseSound();
				SlipstreamingAudio->pauseSound();
			}
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
			} else {// you're slipstreaming
				health += amount;
				SlipstreamingAudio->playSound(stereo.buffer[Audio::SOUND_FILE_HEALING_SFX]);
			}
		} else if (amount < 0) {

			if (health + amount < 0) {// if you would die
				if (health <= 5) {// don't play with fire; you're dead
					health = 0;
					engineAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_DESPAWN_SFX]);
					lockBike();
				} else {// be a merciful god
					health = 1;
				}

			} else {//take damage
				health += amount;
			}
		}
		if (getId() == 0) {//update P1 UI
			UI->updateSI(health);
		}
	}

	//Bike collisions with walls
	void wallCollision() {
		if (getId() == 0) {
			if (collision(getId()) && collisionCD <= 0) {
				modifyHealth(-10);
				collisionCD = 45;
				SlipstreamCD = 30;
				chassisAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BIKE_IMPACT_SFX]);
			}
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

		glm::vec3 heading(
			resetLocation.q.getBasisVector2().x, 
			resetLocation.q.getBasisVector2().y,
			resetLocation.q.getBasisVector2().z
		);

		glm::vec3 cur(glm::normalize(waypoints[waypoint]));

		glm::vec3 to;
		if (waypoint+1 >= waypoints.size()) {
			to = glm::normalize(waypoints[0]);
		} else {
			to = glm::normalize(waypoints[waypoint+1]);
		}

		float angle = glm::atan(to.z - cur.z, to.x - cur.x);

		const float pi = glm::pi<float>();
		if (angle > pi) {
			angle -= 2 * pi;
		} else if (angle < -pi) {
			angle += 2 * pi;
		}

		float temp = 0.0f;
		if (angle < 0) {
			if (angle > -0.75f) {
				temp = 1.15f;
			} else if (angle < -2.75) {
				temp = -1.15f;
			} else {
				temp = pi / 2.0f;
			}
		} else {
			if (angle > 2.75f) {
				temp = 2.75f;
			} else if (angle < 0.75f) {
				temp = -2.75f;
			} else {
				temp = 0.0f;
			}
		}

		resetLocation.q.x = 0;
		resetLocation.q.y = sin(temp);
		resetLocation.q.z = 0;
		resetLocation.q.w = cos(temp);

		resetBikePos(getId(), resetLocation);
		if (resettingCD <= 0) {

			modifyHealth(-25);
		}
		resettingCD += 150;
	}

	//Hitscan bike cannon, the FRAG
	bool fragFire(int bike) {
		float hitDistance;
		auto wallPointer = fragRay(bike, 200, &hitDistance);

		render.instance_set_material(fragIns, fragMat);
		fragCounter = fragCount;

		if (wallPointer != NULL) {
		render.instance_set_trans(fragIns, convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), {1, 1, hitDistance}));
			// Possibly put wall deletion here
			markWallBroken(static_cast<wallUserData*>(wallPointer)->bikeNumber, static_cast<wallUserData*>(wallPointer)->wallIndex);

			FRAGImpactAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT2_SFX]);
			return true;
		} else {
			render.instance_set_trans(fragIns, convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), {1, 1, 200}));
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
				modifyHealth(0.1 * place * Slipstreams);
				Slipstreaming = true;
				SlipstreamingAudio->playSound(stereo.buffer[Audio::SOUND_FILE_HEALING_SFX]);
			}
		} else if (SlipstreamCD < 30) {
			SlipstreamCD++;
			Slipstreaming = false;
			SlipstreamingAudio->pauseSound();
		}
	}

};