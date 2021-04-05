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

  public:
	WallSpawner(Render::Render& render, Render::MaterialHandle wallMaterialHandle) : wall(render, wallMaterialHandle) {}

	bool shouldSpawnWall = false;
	std::optional<float> spawnWad;

	void spawnWalls(float timestep, int id) {
		physx::PxTransform bikeTransform = getBikeTransform(id);
		physx::PxTransform emitTransform = bikeTransform;
		emitTransform.p += emitPoint.x * emitTransform.q.getBasisVector0();
		emitTransform.p += emitPoint.y * emitTransform.q.getBasisVector1();
		emitTransform.p += emitPoint.z * emitTransform.q.getBasisVector2();

		physx::PxTransform wadStart = emitTransform;
		wadStart.p -= wadDepth * wadStart.q.getBasisVector2();
		physx::PxTransform wadEnd = emitTransform;
		wadEnd.p += wadDepth * wadEnd.q.getBasisVector2();

		if (!didSpawnWall && (shouldSpawnWall || spawnWad.has_value())) {
			if (spawnWad.has_value()) {
				auto g = wall.commit_wall(convertTransform(wadStart), scale);
				wall.delete_wall(g);
			} else {
				auto g = wall.commit_wall(convertTransform(emitTransform), scale);
				wall.delete_wall(g);
			}
			timer = 0;
			wallLast = emitTransform;
		}
		if (didSpawnWall && (!shouldSpawnWall || spawnWad.has_value())) {
			if (spawnWad.has_value()) {
				auto g = wall.commit_wall(convertTransform(wadStart), scale);
				makeWallSeg(id, wallLast, wadStart, scale.x, scale.y, {g}, wall);
			} else {
				auto g = wall.commit_wall(convertTransform(emitTransform), scale);
				makeWallSeg(id, wallLast, emitTransform, scale.x, scale.y, {g}, wall);
				wallLast = wadEnd;
			}
			if (!shouldSpawnWall) {
				timer = 0;
			}
		}
		if (didSpawnWall && shouldSpawnWall && !spawnWad.has_value()) {
			timer += timestep;
			if (timer > wallTime) {
				auto g = wall.commit_wall(convertTransform(emitTransform), scale);
				makeWallSeg(id, wallLast, emitTransform, scale.x, scale.y, {g}, wall);
				timer -= wallTime;
				wallLast = emitTransform;
			} else {
				wall.update_wall(convertTransform(emitTransform), scale);
			}
		}

		if (spawnWad.has_value()) {
			auto a = wall.commit_wall(convertTransform(wadStart), {spawnWad.value(), scale.y});
			auto b = wall.commit_wall(convertTransform(wadEnd), {spawnWad.value(), scale.y});
			auto c = wall.commit_wall(convertTransform(wadEnd), scale);
			makeWallSeg(id, wadStart, wadEnd, spawnWad.value(), scale.y, {a, b, c}, wall);
			timer = 0;
			wallLast = wadEnd;
			spawnWad.reset();
		}

		didSpawnWall = shouldSpawnWall;
	}
};