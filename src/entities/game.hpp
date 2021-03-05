#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>

#include "entities/entity_manager.hpp"

#include "entities/bike_player.hpp"
#include "entities/bike_ai.hpp"
#include "entities/checkpoint.hpp"
#include "entities/wall.hpp"
#include "render/model_import.hpp"
#include "render/render.hpp"

#include "physics/physics.h"
#include "window.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct {
	bool operator()(Bike* a, Bike* b) const {
		if (a->getLap() != b->getLap())
			return a->getLap() > b->getLap();
		return a->getCheckpoint() > b->getCheckpoint();
	}
} place_sort;

class Game : public Entity {
  private:
	std::vector<Bike*> bikes;
	int players;
	// std::vector<Checkpoint> checkpoints;
	// Checkpoint finish = new Checkpoint();

	Render::Group car_model;
	Render::Group wall_model;
	Render::Group track_model;

	std::vector<std::vector<glm::vec3>> ai_waypoints; // a list of waypoints for each ai bike

	physx::PxTransform& car_pt;
	physx::PxTransform& wall_pt;

	Window& window;
	Render::Render& render;
	EntityManager& e_manager;

  public:
	Game(
		Window& window, Render::Render& render, int players, EntityManager& em, physx::PxTransform& car_pt,
		physx::PxTransform& wall_pt)
		: window(window), render(render), e_manager(em), players(players), car_pt(car_pt), wall_pt(wall_pt),
		  car_model(importModel("assets/Bike_Final.glb", render)),
		  wall_model(importModel("assets/Wall_blob.glb", render)),
		  track_model(importModel("assets/The_Coffin_render.glb", render))
		  {
		//loading in the AI waypoint vertices with a dummy variable 0 for the player bike id
		//std::vector<glm::vec3> map;
		//ai_waypoints.push_back(map);
		UploadMap("assets/AI_waypoints_1.obj");

	}

	void enter() override { 
		Render::GroupInstance track(track_model);

		std::unique_ptr<Bike> b = std::make_unique<BikePlayer>(window, render, 0, car_pt, car_model);
		bikes.push_back(b.get());
		e_manager.addEntity(std::move(b));

		for (int i = 0; i < players - 1; i++) {
			initVehicle();

			std::unique_ptr<Bike> b = std::make_unique<BikeAI>(render, i, car_pt, car_model, ai_waypoints[i]);
			bikes.push_back(b.get());
			e_manager.addEntity(std::move(b));
		}
		e_manager.addEntity(std::make_unique<Wall>(render, wall_pt, wall_model));
	}

	void update(float deltaTime) override { 
		UpdatePlaces();
	}

	void UpdatePlaces() {
		// updates the places based on car positions/laps
		// this will most likely be implemented by comparing cars by lap
		// if lap is equal compare by checkpoint (divide the track up invisibly)
		// finally if equal again compare by distance to next checkpoint
		
		std::sort(bikes.begin(), bikes.end(), place_sort);
		for (int i = 0; i < bikes.size(); i++) {
			bikes[i]->setPlace(i+1);
		}
	}

	void UploadMap(const char waypoints[]) {
		Assimp::Importer importer;

		//read the file
		const aiScene* scene = importer.ReadFile(waypoints, aiProcess_PreTransformVertices);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		//fetch each vertex of the waypoint mesh
		std::vector<glm::vec3> map;
		for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
			glm::vec3 v;
			v.x = scene->mMeshes[0]->mVertices[i].x;
			v.y = scene->mMeshes[0]->mVertices[i].y;
			v.z = scene->mMeshes[0]->mVertices[i].z;
			map.push_back(v);
		}
		//pushback the ai_map array
		ai_waypoints.push_back(map);
	}
};