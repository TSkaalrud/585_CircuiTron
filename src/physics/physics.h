#pragma once
#include <GLFW/glfw3.h>

void initPhysics();

void stepPhysics(GLFWwindow* window);

void cleanupPhysics();