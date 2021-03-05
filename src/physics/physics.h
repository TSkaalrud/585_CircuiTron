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

void initVehicle();

int getNumBikes();

physx::PxTransform getBikeTransform(int i);

physx::PxTransform getWallPos(int i, int j);

physx::PxTransform getWallFront(int i, int j);

physx::PxTransform getWallBack(int i, int j);

int getNumWalls(int i);

int getWallOwner(int i, int j);

void bikeAccelerate(int i);

void bikeReverse(int i);

void bikeBreak(int i);

void bikeTurnRight(int i);

void bikeTurnLeft(int i);

void bikeTurnPrecise(int i, float n);

void bikeReleaseGas(int i);

void bikeReleaseSteer(int i);

void bikeReleaseBrake(int i);

void bikeReleaseAll(int i);

void initPhysics();

void stepPhysics(physx::PxTransform& player, physx::PxTransform&);

void cleanupPhysics();