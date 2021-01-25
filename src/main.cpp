#include "render/render.hpp"
#include "physics/physics.h"

int main() {

	//Render::render_test();
	initPhysics(true);

	while (1) {
		stepPhysics(true);
	}
	cleanupPhysics(true);

	return 0;
}