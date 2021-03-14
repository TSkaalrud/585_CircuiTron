#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>
#include <PxPhysicsAPI.h>

struct wallSegment {
	int bikeNumber;

	physx::PxRigidStatic* wall;

	physx::PxTransform front;
	physx::PxTransform back;
};

struct wallSpawnInfo {
	physx::PxF32 timer;
	physx::PxF32 wallTime;
	physx::PxTransform wallFront;
	physx::PxTransform wallBack;
};

void initVehicle();

int getNumBikes();

physx::PxVehicleDrive4W* getVehicle(int i);

wallSpawnInfo* getWallInfo(int i);

void makeWallSeg(int i, physx::PxTransform a, physx::PxTransform b);

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

void resetBikePos(int i);

void initPhysics();

void stepPhysics(float timestep);

void cleanupPhysics();