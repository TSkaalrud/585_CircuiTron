#pragma once

#include "game_object.hpp"
#include "wall_manager.hpp"
#include "physics/physics.h"

class Bike : public GameObject {
  private:
	int lap = 1;
	int waypoint = 0;
	int place;
	int health = 100;
	int id;
	bool locked = false;

	WallManager* wm;

  public:
	Bike(Render::Render& render, int start_place, Render::Group& group, WallManager* wm)
		: GameObject(render, group), place(start_place), id(start_place-1), wm(wm){};

	int getId() { return id; }

	void lockBike() { 
		locked = true;
		bikeReleaseAll(id);
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