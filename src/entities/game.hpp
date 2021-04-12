#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>

#include "entities/entity_manager.hpp"

#include "entities/bike_ai.hpp"
#include "entities/bike_player.hpp"
#include "entities/camera.hpp"
#include "entities/track.hpp"
#include "render/model_import.hpp"
#include "render/render.hpp"

#include "physics/physics.h"
#include "window.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include <stdlib.h>

#include "entities/ui_game.h"

#include <chrono>

class Game : public Entity {
  private:
	std::vector<Bike*> order;
	std::vector<Bike*> bikes;
	int players;
	Track* track_pointer;
	Camera* camera_pointer;
	// std::vector<Checkpoint> checkpoints;
	// Checkpoint finish = new Checkpoint();

	std::vector<Render::Group> BikeModels; // a list of bike models
	Render::Group car_model;

	Render::Group wall_model;
	Render::Group track_model;

	std::vector<Render::MaterialHandle> playerWallMaterials;
	Render::MaterialHandle p1_wall;
	Render::MaterialHandle p2_wall;
	Render::MaterialHandle p3_wall;
	Render::MaterialHandle p4_wall;

	std::vector<std::vector<glm::vec3>> ai_waypoints; // a list of waypoints for each ai bike
	std::vector<int> waypointOptions{1, 2, 3, 4, 5, 6, 7, 8, 9};

	Window& window;
	Render::Render& render;
	EntityManager& e_manager;
	UiGame* game_UI;

	Audio::AudioEngine& stereo;

	bool gameover = false;
	bool& menuActive;

  public:
	Game(
		Window& window, Render::Render& render, int players, EntityManager& em, Audio::AudioEngine& audio, UiGame* UI,
		bool& menuActive)
		: window(window), render(render), e_manager(em), players(players), menuActive(menuActive), game_UI(UI),
		  car_model(importModel("assets/Bike_Final.glb", render)),
		  wall_model(importModel("assets/Wall_blob.glb", render)),
		  track_model(importModel("assets/The_Coffin_render.glb", render)), stereo(audio) {
		// loading in the AI waypoint vertices
		uploadMap("assets/AI_waypoints_1.obj");
		uploadMap("assets/AI_waypoints_2.obj");
		uploadMap("assets/AI_waypoints_3.obj");
		uploadMap("assets/AI_waypoints_4.obj");
		uploadMap("assets/AI_waypoints_5.obj");
		uploadMap("assets/AI_waypoints_6.obj");
		uploadMap("assets/AI_waypoints_7.obj");
		uploadMap("assets/AI_waypoints_8.obj");
		uploadMap("assets/AI_waypoints_9.obj");

		// load in the 4 bike models
		uploadBike("assets/Bike_P1.glb", render);
		uploadBike("assets/Bike_P2.glb", render);
		uploadBike("assets/Bike_P3.glb", render);
		uploadBike("assets/Bike_P4.glb", render);
	}

	void enter() override {
		// set the skybox texture asset
		render.set_skybox_rect_texture(importSkybox("assets/skyboxes/SPACE-1.hdr", render));

		// Create and pushback the 4 playerWallMaterials
		Render::MaterialPBR p1_wall_pbr = {
			.albedoFactor = glm::vec4(0.f, 0.000520f, 0.266f, 1.f),
			.metalFactor = 0.5f,
			.roughFactor = 0.05f,
			.emissiveFactor = glm::vec3(0, 0, 0)};
		p1_wall = render.create_pbr_material(p1_wall_pbr);
		playerWallMaterials.push_back(p1_wall);

		Render::MaterialPBR p2_wall_pbr = {
			.albedoFactor = glm::vec4(0.898f, 0.158f, 0.f, 1.f),
			.metalFactor = 0.5f,
			.roughFactor = 0.05f,
			.emissiveFactor = glm::vec3(0, 0, 0)};
		p2_wall = render.create_pbr_material(p2_wall_pbr);
		playerWallMaterials.push_back(p2_wall);

		Render::MaterialPBR p3_wall_pbr = {
			.albedoFactor = glm::vec4(0.f, 0.266f, 0.07f, 1.f),
			.metalFactor = 0.5f,
			.roughFactor = 0.05f,
			.emissiveFactor = glm::vec3(0, 0, 0)};
		p3_wall = render.create_pbr_material(p3_wall_pbr);
		playerWallMaterials.push_back(p3_wall);

		Render::MaterialPBR p4_wall_pbr = {
			.albedoFactor = glm::vec4(0.638f, 0.f, 0.28f, 1.f),
			.metalFactor = 0.5f,
			.roughFactor = 0.05f,
			.emissiveFactor = glm::vec3(0, 0, 0)};
		p4_wall = render.create_pbr_material(p4_wall_pbr);
		playerWallMaterials.push_back(p4_wall);

		// Run the looping bgm and ambiance tracks
		srand((unsigned int)time(NULL));
		AudioInstance* bgm = new AudioInstance();
		bgm->gain = 0.0;
		bgm->playSound(stereo.buffer[rand() % 7 + 101]); // Song
		AudioInstance* ambiance = new AudioInstance();
		ambiance->gain = 0.0;
		ambiance->playSound(stereo.buffer[Audio::SOUND_FILE_AMBIENCE_BGM]); // ambient environment sounds

		// make the track
		std::unique_ptr<Track> t = std::make_unique<Track>(render, track_model);
		track_pointer = t.get();
		e_manager.addEntity(std::move(t));

		// make the player's bike
		std::unique_ptr<Bike> b = std::make_unique<BikePlayer>(
			window, render, 1, BikeModels[0], ai_waypoints[4], stereo, playerWallMaterials[0], game_UI, menuActive);
		bikes.push_back(b.get());
		order.push_back(b.get());

		e_manager.addEntity(std::move(b));

		// make the AI bikes
		for (int i = 0; i < players - 1; i++) {
			initVehicle();

			std::unique_ptr<Bike> b = std::make_unique<BikeAI>(
				window, render, i + 2, BikeModels[i + 1], ai_waypoints, stereo, playerWallMaterials[i + 1], game_UI,
				menuActive, waypointOptions);
			bikes.push_back(b.get());
			order.push_back(b.get());

			e_manager.addEntity(std::move(b));
		}

		// make the camera
		std::unique_ptr c = std::make_unique<Camera>(window, render);
		camera_pointer = c.get();
		e_manager.addEntity(std::move(c));
	}

	void update(float deltaTime) override {
		// check values of UI elements to figure out menu status when pressing enter
		if (!menuActive) {
			if (!gameover) {
				updatePlaces();
				checkWin();
			}
			// menuInput();
		}
		// menuInput();
	}

	void updatePlaces() {
		// updates the places based on car positions/laps
		// this will most likely be implemented by comparing cars by lap
		// if lap is equal compare by checkpoint (divide the track up invisibly)
		// finally if equal again compare by distance to next checkpoint
		std::sort(order.begin(), order.end(), [](Bike* a, Bike* b) {
			if (a->getLap() != b->getLap())
				return a->getLap() > b->getLap();
			return a->getWaypoint() > b->getWaypoint();
		});
		for (int i = 0; i < bikes.size(); i++) {
			order[i]->setPlace(i + 1);
			// std::cout << bikes[0]->getPlace() << std::endl;
			// std::cout << bikes[0]->getPlace() << std::endl;
		}
		// game_UI->currentPlace = bikes[0]->getPlace();
		game_UI->updatePlace(bikes[0]->getPlace());
		game_UI->updateLap(bikes[0]->getLap());
	}

	void checkWin() {
		// currently 3 laps to win
		if (order[0]->getLap() - 1 == 3) {
			if (order[0]->getId() == 0) {
				std::cout << "Player Wins!" << std::endl;
			} else {
				std::cout << "AI " << order[0]->getId() << " Wins!" << std::endl;
			}
			lockAllBikes();
			game_UI->winner(order[0]->getId());
			gameover = true;
		} else {
			game_UI->winner(-1);
		}
	}

	void deleteGame() {
		for (int i = 0; i < bikes.size(); i++) {
			e_manager.removeEntity(bikes[i]);
		}
		e_manager.removeEntity(track_pointer);
		e_manager.removeEntity(camera_pointer);
		e_manager.removeEntity(this);
	}

	void lockAllBikes() {
		for (int i = 0; i < bikes.size(); i++) {
			bikes[i]->lockBike();
		}
	}

	void unlockAllBikes() {
		for (int i = 0; i < bikes.size(); i++) {
			bikes[i]->unlockBike();
		}
	}

	void uploadMap(const char waypoints[]) {
		Assimp::Importer importer;

		// read the file
		const aiScene* scene = importer.ReadFile(waypoints, aiProcess_PreTransformVertices);
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		// fetch each vertex of the waypoint mesh
		std::vector<glm::vec3> map;
		for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
			glm::vec3 v;
			v.x = scene->mMeshes[0]->mVertices[i].x;
			v.y = scene->mMeshes[0]->mVertices[i].y;
			v.z = scene->mMeshes[0]->mVertices[i].z;
			map.push_back(v * 2.0f); // scaled
		}
		// pushback the ai_map array
		ai_waypoints.push_back(map);
	}

	void uploadBike(const char bike[], Render::Render& render) {
		BikeModels.push_back(Render::importModel(bike, render));
	}

	void wallCollision(int i) { bikes[i]->wallCollision(); }

	std::vector<glm::vec3> getWaypoints(int middle) {
		// middle is ai_waypoints[1]
		return ai_waypoints[middle];
	}

	////this input can be checked while bikes are locked
	// void menuInput() {
	//	if (window.keyPressed(256)) { // esc
	//		game_UI->pause();
	//	} // menu
	//	if (game_UI->getMenuActive()) {

	//		if (window.keyPressed(257)) { // enter
	//			game_UI->enterMenuItem();
	//		}
	//		if (window.keyPressed(87) || window.keyPressed(265)) { // w or up
	//			game_UI->selectMenuItem(-1);
	//		}
	//		if (window.keyPressed(83) || window.keyPressed(264)) { // s or down
	//			game_UI->selectMenuItem(1);
	//		}
	//	}
	//}
};