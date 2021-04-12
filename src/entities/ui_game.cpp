#include "ui_game.h"

#include "game.hpp"

void UiGame::enter() {
	// You can pass the transform as a 3rd param
	// I set the transform in update so I don't need to set it here.
	// SI_Bar and SI_Fill make up the SI health bar
	if (SI_Bar == -1) {
		auto position = glm::vec3{1920 * -0.35, 1080 * 0.45, 0};
		auto scale = glm::vec2{401, 41} / 1.5f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		SI_Bar = render.create_instance(render.ui_mesh(), SI_Bar_png, transform);
	}
	if (SI_Fill == -1) {
		auto position = glm::vec3{1920 * -0.35, 1080 * 0.45, 0};
		auto scale = glm::vec2{401, 41} / 1.5f;
		auto depth = 0.1f;
		auto transform = Render::ui_transform(position, scale, depth);

		SI_Fill = render.create_instance(render.ui_mesh(), SI_Secure_png, transform);
	}
	// Lap_UI and Lap_Num make up the Lap tracker UI
	if (Lap_UI == -1) {
		auto position = glm::vec3{1920 * -0.4, 1080 * -0.4, 0};
		auto scale = glm::vec2{127, 71} / 2.0f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Lap_UI = render.create_instance(render.ui_mesh(), UI_Lap_png, transform);
	}
	if (Lap_Num == -1) {
		auto position = glm::vec3{1920 * -0.4, 1080 * -0.45 - 15, 0};
		auto scale = glm::vec2{25, 51} / 2.0f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Lap_Num = render.create_instance(render.ui_mesh(), First_png, transform);
	}
	// Place_UI and Place_Num make up the position tracker
	if (Place_UI == -1) {
		auto position = glm::vec3{1920 * 0.4, 1080 * -0.4, 0};
		auto scale = glm::vec2{127, 59} / 2.0f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Place_UI = render.create_instance(render.ui_mesh(), UI_Place_png, transform);
	}
	if (Place_Num == -1) {
		auto position = glm::vec3{1920 * 0.4, 1080 * -0.45 - 9, 0};
		auto scale = glm::vec2{25, 51} / 2.0f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Place_Num = render.create_instance(render.ui_mesh(), Fourth_png, transform);
	}
	// Winner shows who won the race at the end
	if (Winner == -1) {
		auto position = glm::vec3{0, -1080, 0} / 6.0f;
		auto scale = glm::vec2{212, 51} / 2.0f;
		auto depth = 0.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Winner = render.create_instance(render.ui_mesh(), You_Win_png, transform);
	}
	// The menu background, this is opaque/has no transparency*
	if (Background == -1) {
		auto position = glm::vec3{0, 0, 0} / 4.0f;
		auto scale = glm::vec2{1920, 1080} / 2.0f;
		auto depth = 0.5f;
		auto transform = Render::ui_transform(position, scale, depth);

		Background = render.create_instance(render.ui_mesh(), Background_png, transform);
	}
	// CircuiTron
	if (TitleCard == -1) {
		auto position = glm::vec3{0, 0, 0} / 4.0f;
		auto scale = glm::vec2{1440, 176} / 2.0f;
		auto depth = 1.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		TitleCard = render.create_instance(render.ui_mesh(), Circuitron_Title_png, transform);
	}
	// Menu_Items 1-3 make up the 3 locations for menu buttons to be placed
	if (Menu_Item_1 == -1) {
		auto position = glm::vec3{0, 1080, 0} / 6.0f;
		auto scale = glm::vec2{315, 94} / 2.0f;
		auto depth = 1.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Menu_Item_1 = render.create_instance(render.ui_mesh(), Play_H_png, transform);
	}
	if (Menu_Item_2 == -1) {
		auto position = glm::vec3{0, 1080, 0} / 4.0f;
		auto scale = glm::vec2{315, 94} / 2.0f;
		auto depth = 1.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Menu_Item_2 = render.create_instance(render.ui_mesh(), Options_U_png, transform);
	}
	if (Menu_Item_3 == -1) {
		auto position = glm::vec3{0, 1080, 0} / 3.0f;
		auto scale = glm::vec2{315, 94} / 2.0f;
		auto depth = 1.0f;
		auto transform = Render::ui_transform(position, scale, depth);

		Menu_Item_3 = render.create_instance(render.ui_mesh(), Exit_U_png, transform);
	}
	// Instructions covers the span of the screen during pause/loading
	if (Instructions == -1) {
		auto position = glm::vec3{0, 0, 0} / 4.0f;
		auto scale = glm::vec2{1920, 1080} / 2.0f;
		auto depth = 0.6f;
		auto transform = Render::ui_transform(position, scale, depth);

		Instructions = render.create_instance(render.ui_mesh(), Instructions_png, transform);
		render.instance_set_material(Instructions, -1);
	}
}

void UiGame::update(float) {

	loadGame();
	menuInput();
}

void UiGame::loadGame() {
	if (!renderLoadingDone) {
		if (!physicsLoadingDone) {
			initPhysics();
			physicsLoadingDone = true;

			std::unique_ptr<Game> g = std::make_unique<Game>(window, render, 4, e_manager, stereo, this, menuActive);
			game_pointer = g.get();
			e_manager.addEntity(std::move(g));
		} else {// loading screen finished
			renderLoadingDone = true;
			render.instance_set_material(Menu_Item_1, Play_R_png);
			render.instance_set_material(Menu_Item_2, -1);
			render.instance_set_material(Menu_Item_3, -1);
		}
	}
}

void UiGame::selectMenuItem(int direction) {
	if (direction == 1) { // switch downwards
		if (currentlySelectedMenuItem == 3) {
			currentlySelectedMenuItem = 1;
		} else
			currentlySelectedMenuItem++;
	} else if (direction == -1) { // switch upwards
		if (currentlySelectedMenuItem == 1) {
			currentlySelectedMenuItem = 3;
		} else
			currentlySelectedMenuItem--;
	}
	// std::cout << currentlySelectedMenuItem << std::endl;

	if (currentlyActiveMenu == 1) {           // main menu
		if (currentlySelectedMenuItem == 1) { // Play
			render.instance_set_material(Menu_Item_1, Play_H_png);
			render.instance_set_material(Menu_Item_2, Options_U_png);
			render.instance_set_material(Menu_Item_3, Exit_U_png);

		} else if (currentlySelectedMenuItem == 2) { // Options
			render.instance_set_material(Menu_Item_1, Play_U_png);
			render.instance_set_material(Menu_Item_2, Options_H_png);
			render.instance_set_material(Menu_Item_3, Exit_U_png);

		} else if (currentlySelectedMenuItem == 3) { // Exit
			render.instance_set_material(Menu_Item_1, Play_U_png);
			render.instance_set_material(Menu_Item_2, Options_U_png);
			render.instance_set_material(Menu_Item_3, Exit_H_png);
		}
	} else if (currentlyActiveMenu == 2) { // Options (currently no options
		if (currentlySelectedMenuItem == 1) {
			render.instance_set_material(Menu_Item_1, -1);
			render.instance_set_material(Menu_Item_2, -1);
			render.instance_set_material(Menu_Item_3, Main_Menu_H_png);

		} else if (currentlySelectedMenuItem == 2) {
			render.instance_set_material(Menu_Item_1, -1);
			render.instance_set_material(Menu_Item_2, -1);
			render.instance_set_material(Menu_Item_3, Main_Menu_H_png);

		} else if (currentlySelectedMenuItem == 3) {
			render.instance_set_material(Menu_Item_1, -1);
			render.instance_set_material(Menu_Item_2, -1);
			render.instance_set_material(Menu_Item_3, Main_Menu_H_png);
		}
	} else if (currentlyActiveMenu == 3) { // loading screen
		render.instance_set_material(Menu_Item_1, Loading_png);
		render.instance_set_material(Menu_Item_2, -1);
		render.instance_set_material(Menu_Item_3, -1);

	} else if (currentlyActiveMenu == 4) { // pause screen uses menu item 1 for pause text
		if (currentlySelectedMenuItem == 1) {
			currentlySelectedMenuItem = 2;
		}
		if (currentlySelectedMenuItem == 2) {
			render.instance_set_material(Menu_Item_1, Paused_png);
			render.instance_set_material(Menu_Item_2, Play_H_png);
			render.instance_set_material(Menu_Item_3, Main_Menu_U_png);

		} else if (currentlySelectedMenuItem == 3) {
			render.instance_set_material(Menu_Item_1, Paused_png);
			render.instance_set_material(Menu_Item_2, Play_U_png);
			render.instance_set_material(Menu_Item_3, Main_Menu_H_png);
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
// activates a menu selection
void UiGame::enterMenuItem() {

	if (currentlyActiveMenu == 1) { // main menu

		menuActive = true;
		paused = false;

		if (currentlySelectedMenuItem == 1) { // Play
			// go to loading page
			currentlyActiveMenu = 3;
			render.instance_set_material(Instructions, Instructions_png);
			render.instance_set_material(TitleCard, -1);
			menuActive = true;
			paused = true;
			// setting these to false will trigger loadGame() in update to build the game
			physicsLoadingDone = false;
			renderLoadingDone = false;
		} else if (currentlySelectedMenuItem == 2) { // Options
			// go to options page
			currentlyActiveMenu = 2;

		} else if (currentlySelectedMenuItem == 3) { // Exit
			window.close();
		}
	} else if (currentlyActiveMenu == 2) { // Options
		render.instance_set_material(Background, Background_png);
		render.instance_set_material(TitleCard, Circuitron_Title_png);

		if (currentlySelectedMenuItem == 1) { // since no options in, they all just default to main menu
			// option 1
			currentlyActiveMenu = 1;

		} else if (currentlySelectedMenuItem == 2) {
			// option 2
			currentlyActiveMenu = 1;

		} else if (currentlySelectedMenuItem == 3) {
			// go to main menu
			currentlyActiveMenu = 1;
		}
	} else if (currentlyActiveMenu == 3) { // loading screen
		render.instance_set_material(Background, -1);
		render.instance_set_material(TitleCard, -1);
		updateSI(100);
		unpause(); // player must have hit the play button/enter to start the game
	} else if (currentlyActiveMenu == 4) { // pause screen uses menu item 1 for pause text
		render.instance_set_material(Background, -1);
		render.instance_set_material(TitleCard, -1);

		if (currentlySelectedMenuItem == 1) {
			currentlySelectedMenuItem = 2;
		}
		if (currentlySelectedMenuItem == 2) {
			// Play
			currentlyActiveMenu = 0;
			menuActive = false;
			paused = false;

		} else if (currentlySelectedMenuItem == 3) {
			// Main Menu
			currentlyActiveMenu = 1;
			game_pointer->deleteGame();
			game_pointer = nullptr;
			cleanupPhysics();

		}
	}
	toggleGameUI();
	selectMenuItem(0);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void UiGame::toggleGameUI() {

	if (currentlyActiveMenu == 1 || currentlyActiveMenu == 2) { // if main menu/options menuActive && !paused
		// toggle off
		render.instance_set_material(SI_Bar, -1);
		render.instance_set_material(SI_Fill, -1);
		render.instance_set_material(Place_Num, -1);
		render.instance_set_material(Place_UI, -1);
		render.instance_set_material(Lap_Num, -1);
		render.instance_set_material(Lap_UI, -1);
		render.instance_set_material(Instructions, -1);
		render.instance_set_material(Winner, -1);
		// toggle on
		render.instance_set_material(Background, Background_png);
		render.instance_set_material(TitleCard, Circuitron_Title_png);
	} else if (currentlyActiveMenu == 3) { // loading screen !menuActive && paused
		// toggle off
		render.instance_set_material(SI_Bar, -1);
		render.instance_set_material(SI_Fill, -1);
		render.instance_set_material(Place_Num, -1);
		render.instance_set_material(Place_UI, -1);
		render.instance_set_material(Lap_Num, -1);
		render.instance_set_material(Lap_UI, -1);
		render.instance_set_material(Winner, -1);
		render.instance_set_material(TitleCard, -1);
		render.instance_set_material(Menu_Item_2, -1);
		render.instance_set_material(Menu_Item_3, -1);
		// toggle on

	} else if (currentlyActiveMenu == 4) { // paused menuActive && paused
		// toggle off
		render.instance_set_material(TitleCard, -1);
		render.instance_set_material(Background, -1);
		// toggle on

	} else if (!menuActive && !paused) { // game on
		// toggle off
		render.instance_set_material(Menu_Item_1, -1);
		render.instance_set_material(Menu_Item_2, -1);
		render.instance_set_material(Menu_Item_3, -1);
		render.instance_set_material(Instructions, -1);
		render.instance_set_material(TitleCard, -1);
		render.instance_set_material(Background, -1);
		// toggle on
		render.instance_set_material(SI_Bar, SI_Bar_png);
		render.instance_set_material(SI_Fill, SI_Secure_png);
		updatePlace(currentPlace);
		render.instance_set_material(Place_UI, UI_Place_png);
		updateLap(currentLap);
		render.instance_set_material(Lap_UI, UI_Lap_png);
	}
}