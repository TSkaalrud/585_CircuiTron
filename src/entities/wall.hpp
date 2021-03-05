#pragma once

#include "game_object.hpp"

class Wall : public GameObject {
  private:
	int numWalls[4] {0, 0, 0, 0};
  public:
	Wall(Render::Render& render, Render::Group& group)
		: GameObject(render, group) {
	};

	void update(float deltaTime) override {

		for (int i = 0; i < getNumBikes(); i++) {
			if (numWalls[i] < getNumWalls(i)) {
				Render::GroupInstance newWall(group);
				newWall.setTransform(convertTransform(getWallPos(i, getNumWalls(i)-1)));
				numWalls[i]++;
			}
		}
	}
};