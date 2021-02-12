#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>


void initPhysics(physx::PxTransform& player);

void stepPhysics(GLFWwindow* window, physx::PxTransform& player);

void cleanupPhysics();