#include "entity_manager.hpp"
#include <vector>

EntityManager::EntityManager() {}

void EntityManager::addEntity(std::unique_ptr<Entity> e) {
	entities_to_add.push_back(std::move(e));
}

void EntityManager::addEntitiesAfterFrame() {
	for (int i = 0; i < entities_to_add.size(); i++) {
		entities.push_back(std::move(entities_to_add.at(i)));
		entities.back()->enter();
	}

	entities_to_add.clear();
}

void EntityManager::update(float dTime) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->update(dTime);
	};
}