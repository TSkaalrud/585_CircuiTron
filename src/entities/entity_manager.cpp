#include "entity.cpp"
#include <vector>

class EntityManager {
	private:
		std::vector<Entity*> entities;

	public:
		void addEntity(Entity* e) { 
			entities.push_back(e); 
		};

		void update(float dTime) {
			for (int i = 0; i < entities.size(); i++) {
				entities[i]->update(dTime);
			};
		};
};