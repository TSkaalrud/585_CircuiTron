#pragma once

#include <algorithm>
#include <vector>

#include "entities/entity_manager.hpp"

#include "entities/car_player.hpp"
#include "entities/checkpoint.hpp"
#include "entities/wall.hpp"
#include "render/model_import.hpp"
#include "render/render.hpp"

struct {
	bool operator()(Car* a, Car* b) const {
		if (a->getLap() != b->getLap())
			return a->getLap() > b->getLap();
		return a->getCheckpoint() > b->getCheckpoint();
	}
} place_sort;

class Game : public Entity {
  private:
	std::vector<Car*> cars;
	int players;
	// std::vector<Checkpoint> checkpoints;
	// Checkpoint finish = new Checkpoint();

	Render::Group car_model;
	Render::Group wall_model;
	Render::Group track_model;

	physx::PxTransform& car_pt;
	physx::PxTransform& wall_pt;

	Render::Render& render;
	EntityManager& e_manager;

  public:
	Game(Render::Render& render, int players, EntityManager& em, physx::PxTransform& car_pt, physx::PxTransform& wall_pt)
		: render(render), e_manager(em), players(players), car_pt(car_pt), wall_pt(wall_pt),
		  car_model(importModel("assets/Bike_Final.glb", render)),
		  wall_model(importModel("assets/Wall_blob.glb", render)),
		  track_model(importModel("assets/The_Coffin_render.glb", render)) {}

	void enter() override { 
		Render::GroupInstance track(track_model);

		for (int i = 0; i < players; i++) {
			std::unique_ptr<Car> c = std::make_unique<CarPlayer>(render, i, car_pt, car_model);
			cars.push_back(c.get());
			e_manager.addEntity(std::move(c));
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

		
		std::sort(cars.begin(), cars.end(), place_sort);
		for (int i = 0; i < cars.size(); i++) {
			cars[i]->setPlace(i+1);
		}
	}
};