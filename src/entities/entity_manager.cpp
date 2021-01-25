#include "entity_manager.hpp"
#include <vector>

EntityManager::EntityManager() {}

void EntityManager::addEntity(Entity* e) { entities.push_back(e); }

void EntityManager::update(float dTime) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->update(dTime);
	};
}