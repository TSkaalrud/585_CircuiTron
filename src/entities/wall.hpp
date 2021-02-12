#pragma once

#include "game_object.hpp"

class Wall : public GameObject {
  private:

  public:
	// change assets/test.glb to car model path
	Wall(Render::Render& render, physx::PxTransform& pt)
		: GameObject(render, "assets/Wall_blob.glb", pt) {};

	void enter() override {
		Render::Group group = importModel("assets/Wall_blob.glb", render);
		model.emplace(group);
	}

	void update(float deltaTime) override { 
		model->setTransform(convertTransform(transform));
	}
};