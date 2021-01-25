#include "entity.hpp"
#include <vector>

class EntityManager {
  private:
	std::vector<Entity*> entities;

  public:
	EntityManager();

	void addEntity(Entity* e);

	void update(float dTime);
};