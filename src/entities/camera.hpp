#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include "physics/physics.h"
#include "window.hpp"


class Camera : public Entity {
  private:
	Window& window;
	Render::Render& render;

	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	float smooth_spd = 0.125f;

	int view_state = 0;
  public:
	Camera(Window& window, Render::Render& render) : Entity(), window(window), render(render){};

	void enter() override {
		render.camera_set_fov(50);
	}  

	void update(float deltaTime) override { 
		
		checkInput();

		if (view_state == 0) {
			behindBike(0);
		} else {
			frontBike(0);
		}
	}

	void behindBike(int i) {
		//physx::PxTransform camera(0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));
		//render.camera_set_pos(convertTransform(getBikeTransform(i).transform(camera)));

		glm::vec3 bike_pos(getBikeTransform(i).p.x, getBikeTransform(i).p.y, getBikeTransform(i).p.z);
		glm::vec3 offset(0.0f, 5.0f, 0.0f);

		glm::vec3 bike_heading = glm::vec3(
			getBikeTransform(i).q.getBasisVector2().x,
			getBikeTransform(i).q.getBasisVector2().y,
			getBikeTransform(i).q.getBasisVector2().z);

		glm::vec3 u = glm::normalize(bike_heading);
		glm::vec3 desired_pos = bike_pos - u * 20.0f + offset;
		
		//glm::vec3 smoothed_pos = lerp(pos, desired_pos, smooth_spd);
		pos = desired_pos;

		glm::mat4 view = glm::lookAt(
			pos,
			bike_pos,
			glm::vec3(0.0f, 1.0f, 0.0f));

		render.camera_set_pos(glm::inverse(view));
	}

	void frontBike(int i) {
		glm::vec3 bike_pos(getBikeTransform(i).p.x, getBikeTransform(i).p.y, getBikeTransform(i).p.z);
		glm::vec3 offset(0.0f, 5.0f, 0.0f);

		glm::vec3 bike_heading = glm::vec3(
			getBikeTransform(i).q.getBasisVector2().x, getBikeTransform(i).q.getBasisVector2().y,
			getBikeTransform(i).q.getBasisVector2().z);

		glm::vec3 u = glm::normalize(bike_heading);
		glm::vec3 desired_pos = bike_pos + u * 20.0f + offset;

		//glm::vec3 smoothed_pos = lerp(pos, desired_pos, smooth_spd);
		pos = desired_pos;

		glm::mat4 view = glm::lookAt(pos, bike_pos, glm::vec3(0.0f, 1.0f, 0.0f));

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
		} else {
			view_state = 0;
		}
	}


	glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) { return x * (1.f - t) + y * t; }
};