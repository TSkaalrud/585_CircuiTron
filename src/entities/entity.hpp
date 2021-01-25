#pragma once

class Entity {
  public:
	Entity();

	virtual void draw();
	virtual void update(float dTime);
};