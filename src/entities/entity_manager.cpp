#include "entity_manager.hpp"
#include <vector>

EntityManager::EntityManager() {}

void EntityManager::addEntity(std::unique_ptr<Entity> e) {
	entities.push_back(std::move(e));
	entities.back()->enter();
}

void EntityManager::update(float dTime) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->update(dTime);
	};
}