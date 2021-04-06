#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>
#include <PxPhysicsAPI.h>
#include <vector>
#include "render/wall.hpp"

struct wallUserData {
	int bikeNumber;
	int wallIndex;
	int collisions;

	physx::PxRigidStatic* wall;

	physx::PxTransform front;
	physx::PxTransform back;

	std::vector<uint32_t> graphicIndex;
	Render::Wall& graphicReference;

	bool broken;
};

struct bikeUserData {
	int bikeNumber;
	bool collided;
};

void initVehicle();

int getNumBikes();

float getSpeed(int i);

physx::PxVehicleDrive4W* getVehicle(int i);

void makeWallSeg(
	int i, physx::PxTransform a, physx::PxTransform b, float width, float height, std::vector<uint32_t> graphicIndex,
	Render::Wall& graphicReference);

void markWallBroken(int i, int j);

physx::PxRaycastBuffer* castRay(int bike, int dir, int range);

bool slipstreamRay(int bike, int dir, int range);

void* fragRay(int bike, int range);

bool slipstreamRay(int bike, int dir, int range);

bool collision(int bike);

physx::PxTransform getBikeTransform(int i);

physx::PxTransform getWallPos(int i, int j);

physx::PxTransform getWallFront(int i, int j);

physx::PxTransform getWallBack(int i, int j);

physx::PxTransform getTrackTransform();

int getNumWalls(int i);

int getWallOwner(int i, int j);

void bikeAccelerate(int i);

void bikeAcceleratePrecise(int i, float n);

void bikeReverse(int i);

void bikeHandbrake(int i);

void bikeBreak(int i);

void bikeTurnRight(int i);

void bikeTurnLeft(int i);

void bikeTurnPrecise(int i, float n);

void bikeReleaseGas(int i);

void bikeReleaseSteer(int i);

void bikeReleaseBrake(int i);

void bikeReleaseHandbrake(int i);

void bikeReleaseAll(int i);

void bikeBooster(int i, int keyPressed);

void bikeControl(int i);

void resetBikePos(int i, physx::PxTransform position);

physx::PxU32 getBikeGear(int bike);

void initPhysics();

void stepPhysics(float timestep);

void cleanupPhysics();