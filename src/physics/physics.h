#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>
#include <PxPhysicsAPI.h>

void initVehicle();

physx::PxTransform getBikeTransform(physx::PxVehicleDrive4W* bike);

physx::PxTransform getWallTransform(physx::PxRigidStatic* wall);

void initPhysics();

void stepPhysics(GLFWwindow* window, physx::PxTransform& player, physx::PxTransform&);

void cleanupPhysics();