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

	for (int i = 0; i < entities_to_remove.size(); i++) {
		for (int j = 0; j < entities.size(); j++) {
			if (entities_to_remove[i] == entities[j].get()) {
				entities[j].reset();
				entities.erase(entities.begin() + j);
				break;
			}
		}
	}

	entities_to_add.clear();
	entities_to_remove.clear();
}

void EntityManager::update(float dTime) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->update(dTime);
	};
}

void EntityManager::removeEntity(Entity* e) { entities_to_remove.push_back(e); }