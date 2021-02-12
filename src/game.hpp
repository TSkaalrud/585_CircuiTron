#include <vector>
#include <algorithm>
#include "entities/car.hpp"
#include "entities/checkpoint.hpp"
#include "render/render.hpp"

class Game {
	private:
		std::vector<Car> cars;
		std::vector<Checkpoint> checkpoints;
		//Checkpoint finish = new Checkpoint();
	
	public:
		Game(Render::Render& render, int players) {
			for (int i = 0; i < players; i++) {
				cars.push_back(new Car(render, i))
			}
		}

		void UpdatePlaces() {
			// updates the places based on car positions/laps
			// this will most likely be implemented by comparing cars by lap
			// if lap is equal compare by checkpoint (divide the track up invisibly)
			// finally if equal again compare by distance to next checkpoint

			bool sorted = true
			std::sort(cars.begin(), cars.end(), lessThanLap);

			for (int i = 0; i < cars.size()-1; i++) {
				if (cars[i].getLap() == cars[i + 1].getLap()) {
					sorted = false;
					break;
				}
			}

			// THIS DOES NOT WORK YET
		}
};