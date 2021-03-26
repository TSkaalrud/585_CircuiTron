#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include "physics/physics.h"
#include "window.hpp"

#include <iostream>


class Camera : public Entity {
  private:
	Window& window;
	Render::Render& render;

	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	float smooth_spd = 0.05f;

	int view_state = 0;
	bool switched = true;

	float fov = 50.0f;
	float desired_fov = 50.0f;
	
	float last_speed = 0.0f;

	float u_dist = 20.0f;

  public:
	Camera(Window& window, Render::Render& render) : Entity(), window(window), render(render){};

	void enter() override {
		render.camera_set_fov(fov);
	}  

	void update(float deltaTime) override { 
		
		checkInput();

		if (view_state == 0) {
			behindBike(0, switched);
		} else {
			frontBike(0);
		}
	}

	void behindBike(int i, bool switch_flag) {
		//physx::PxTransform camera(0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));
		//render.camera_set_pos(convertTransform(getBikeTransform(i).transform(camera)));

		glm::vec3 bike_pos(getBikeTransform(i).p.x, getBikeTransform(i).p.y, getBikeTransform(i).p.z);
		glm::vec3 offset(0.0f, 5.0f, 0.0f);

		glm::vec3 bike_heading = glm::vec3(
			getBikeTransform(i).q.getBasisVector2().x,
			getBikeTransform(i).q.getBasisVector2().y,
			getBikeTransform(i).q.getBasisVector2().z);

		glm::vec3 u = glm::normalize(bike_heading);

		//updateOffset();

		glm::vec3 desired_pos = bike_pos - u * u_dist + offset;
		
		//so that front view doesnt lerp
		if (switch_flag) {
			pos = desired_pos;
			switched = false;
		} else {
			updateFOV();
			glm::vec3 smoothed_pos = lerp(pos, desired_pos, smooth_spd);
			pos = smoothed_pos;
		}

		glm::mat4 view = glm::lookAt(pos, bike_pos, glm::vec3(0.0f, 1.0f, 0.0f));

		render.camera_set_fov(fov);
		render.camera_set_pos(glm::inverse(view));
	}

	//same as behind bike but a positive u
	void frontBike(int i) {
		glm::vec3 bike_pos(getBikeTransform(i).p.x, getBikeTransform(i).p.y, getBikeTransform(i).p.z);
		glm::vec3 offset(0.0f, 5.0f, 0.0f);

		glm::vec3 bike_heading = glm::vec3(
			getBikeTransform(i).q.getBasisVector2().x, getBikeTransform(i).q.getBasisVector2().y,
			getBikeTransform(i).q.getBasisVector2().z);

		glm::vec3 u = glm::normalize(bike_heading);
		glm::vec3 desired_pos = bike_pos + u * 20.0f + offset;		//the difference
		
		//also no lerping
		pos = desired_pos;

		glm::mat4 view = glm::lookAt(pos, bike_pos, glm::vec3(0.0f, 1.0f, 0.0f));

		updateFOV();
		render.camera_set_fov(50.0f);
		render.camera_set_pos(glm::inverse(view));
	}

	void topDown() { 
		pos = glm::vec3(1.0f, 250.0f, 50.0f);
		glm::vec3 bike_pos(getBikeTransform(1).p.x, getBikeTransform(1).p.y, getBikeTransform(1).p.z);
		
		glm::mat4 view = glm::lookAt(pos, bike_pos, glm::vec3(0.0f, 1.0f, 0.0f));
		render.camera_set_pos(glm::inverse(view));
	}

	void checkInput() {
		if (window.keyPressed(86)) { // v
			view_state = 1;
			switched = true;
		} else {
			view_state = 0;
		}
	}

	void updateFOV() { 
		physx::PxF32 g = getBikeGear(0);
		float gearRange = (6.0f - 1.0f);
		
		// min - max -- use to modify how much fov changes
		float fovRange = (40.0f - 60.0f);

		if (g != 1.0f) {
			desired_fov = (((g - 1.0f) * fovRange) / gearRange) + 60.0f;
		}
		
		// modify the last float (0 - 1) to change how smoothly the fovs change
		fov = lerp(fov, desired_fov, 0.01);

		//std::cout << fov << std::endl;
	}

	void updateOffset() {
		physx::PxF32 g = getBikeGear(0);
		float gearRange = (6.0f - 1.0f);

		float newOffset = 20.0f;

		if (g != 1.0f) {
			float offsetRange = (17.0f - 20.0f);
			newOffset = (((g - 1.0f) * offsetRange) / gearRange) + 20.0f;
		}

		u_dist = newOffset;

		std::cout << u_dist << std::endl;
	}

	glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) { return x * (1.f - t) + y * t; }

	float lerp(float a, float b, float t) { return a + t * (b - a); }
};