#include "entity.hpp"
#include <memory>
#include <vector>

class EntityManager {
  private:
	std::vector<std::unique_ptr<Entity>> entities;

  public:
	EntityManager();

	void addEntity(std::unique_ptr<Entity> e);

	void update(float dTime);
};