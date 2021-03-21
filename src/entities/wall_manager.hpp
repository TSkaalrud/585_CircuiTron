#pragma once

#include "game_object.hpp"
#include "physics/physics.h"
#include <list>

class WallManager : public GameObject {
  private:
	int id;

	std::list<std::unique_ptr<Render::GroupInstance>> walls; 
  public:
	WallManager(Render::Render& render, Render::Group& group, int id)
		: GameObject(render, group), id(id) {
	};

	void enter() override {}

	void update(float deltaTime) override {}

	void addWall() {
		std::unique_ptr<Render::GroupInstance> wall = std::make_unique<Render::GroupInstance>(group);

		walls.emplace_back(std::move(wall));
		walls.back()->setTransform(convertTransform(getWallPos(id, getNumWalls(id) - 1)));
	}
};