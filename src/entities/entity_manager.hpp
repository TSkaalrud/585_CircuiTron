#include "entity.hpp"
#include <memory>
#include <vector>

class EntityManager {
  private:
	std::vector<std::unique_ptr<Entity>> entities;
	std::vector<std::unique_ptr<Entity>> entities_to_add;

  public:
	EntityManager();

	void addEntity(std::unique_ptr<Entity> e);
	void addEntitiesAfterFrame();

	void update(float dTime);
};