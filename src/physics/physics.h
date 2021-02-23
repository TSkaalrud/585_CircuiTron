#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>
#include <PxPhysicsAPI.h>

void initVehicle();

physx::PxTransform getBikeTransform(int i);

physx::PxTransform getWallTransform(int i, int j);

void bikeAccelerate(int i);

void bikeReverse(int i);

void bikeBreak(int i);

void bikeTurnRight(int i);

void bikeTurnLeft(int i);

void bikeReleaseGas(int i);

void bikeReleaseSteer(int i);

void bikeReleaseBrake(int i);

void bikeReleaseAll(int i);

void initPhysics();

void stepPhysics(physx::PxTransform& player, physx::PxTransform&);

void cleanupPhysics();