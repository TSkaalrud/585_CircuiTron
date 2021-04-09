#include "ui_game.h"

// activates a menu selection
void UiGame::enterMenuItem() {
	// reset bikes, health, race,

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
			// reset the game here---------------------------------------------------------
			// initialize game
			e_manager.addEntity(std::make_unique<Game>(window, render, 4, e_manager, stereo, this, menuActive));
		} else if (currentlySelectedMenuItem == 2) { // Options
			// go to options page
			currentlyActiveMenu = 2;
		} else if (currentlySelectedMenuItem == 3) { // Exit
			// close app--------------------------------------------------------------------
			exit(1);
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
		// player must have hit the play button/enter to start the game
		unpause();
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
		}
	}
	toggleGameUI();
	selectMenuItem(0);
	// currentlySelectedMenuItem = 1;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}