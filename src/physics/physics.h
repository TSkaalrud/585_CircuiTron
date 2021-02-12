#pragma once
#include <GLFW/glfw3.h>
#include <foundation/PxTransform.h>
#include <foundation/Px.h>


void initPhysics();

void stepPhysics(GLFWwindow* window, physx::PxTransform& player, physx::PxTransform&);

void cleanupPhysics();