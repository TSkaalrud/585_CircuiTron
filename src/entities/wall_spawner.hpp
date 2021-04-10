#pragma once

#include "physics/physics.h"

class WallSpawner {
  private:
	const glm::vec3 emitPoint = {0, 0, -4.8};
	const float wadDepth = 0.1;
	const glm::vec2 scale = {0.1, 1};

	float timer = 0.0f;
	const float wallTime = 10.0f / 60.0f;
	physx::PxTransform wallLast;
	bool didSpawnWall = false;

	Render::Wall wall;

	physx::PxTransform lastFrame;

	physx::PxTransform offsetTransform(physx::PxTransform t, glm::vec3 o) {
		physx::PxTransform result = t;
		result.p += o.x * result.q.getBasisVector0();
		result.p += o.y * result.q.getBasisVector1();
		result.p += o.z * result.q.getBasisVector2();
		return result;
	}

  public:
	WallSpawner(Render::Render& render, Render::MaterialHandle wallMaterialHandle) : wall(render, wallMaterialHandle) {}

	bool shouldSpawnWall = false;
	std::optional<float> spawnWad;

	void spawnWalls(float timestep, int id) {

		if (didSpawnWall && shouldSpawnWall && !spawnWad.has_value()) {
			timer += timestep;
		}

		if (didSpawnWall || shouldSpawnWall || spawnWad.has_value()) {
			auto emitTransform = offsetTransform(getBikeTransform(id), emitPoint);

			physx::PxTransform firstSegEnd = emitTransform;
			if (didSpawnWall && !shouldSpawnWall) {
				firstSegEnd = lastFrame;
			}
			lastFrame = emitTransform;
			if (spawnWad.has_value()) {
				firstSegEnd.p += -wadDepth * firstSegEnd.q.getBasisVector2();
			}

			if (!didSpawnWall || !shouldSpawnWall || spawnWad.has_value() || timer >= wallTime) {

				if (firstSegEnd != wallLast) {
					auto g = wall.commit_wall(convertTransform(firstSegEnd), scale);
					if (didSpawnWall) {
						makeWallSeg(id, wallLast, firstSegEnd, scale.x, scale.y, {g}, wall);
					} else {
						wall.delete_wall(g);
					}
				}

				timer = 0;
				wallLast = emitTransform;
				if (spawnWad.has_value()) {
					wallLast.p += wadDepth * wallLast.q.getBasisVector2();
				}

				if (spawnWad.has_value()) {
					auto a = wall.commit_wall(convertTransform(firstSegEnd), {spawnWad.value(), scale.y});
					auto b = wall.commit_wall(convertTransform(wallLast), {spawnWad.value(), scale.y});
					auto c = wall.commit_wall(convertTransform(wallLast), scale);
					makeWallSeg(id, firstSegEnd, wallLast, spawnWad.value(), scale.y, {a, b, c}, wall);
					spawnWad.reset();
				}
			} else {
				wall.update_wall(convertTransform(firstSegEnd), scale);
			}
		}

		didSpawnWall = shouldSpawnWall;
	}
};