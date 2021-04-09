#pragma once

#include "entities/entity.hpp"
#include "render/import.hpp"
#include "render/ui_util.hpp"
#include "window.hpp"
#include <chrono>
#include <thread>
#include "Audio/audioEngine.h"
#include "game.hpp"

class UiGame : public Entity {

#pragma region Init
  private:
	Render::Render& render;
	Window& window;
	bool& menuActive;
	EntityManager& e_manager;
	Audio::AudioEngine& stereo;

	bool paused;
	int currentlySelectedMenuItem = 1;//of 3
	int currentlyActiveMenu = 1;//of 3
	

	//UI instanceHandles
	Render::InstanceHandle SI_Bar = -1;
	Render::InstanceHandle SI_Fill = -1;
	Render::InstanceHandle Lap_UI = -1;
	Render::InstanceHandle Lap_Num = -1;
	Render::InstanceHandle Place_UI = -1;
	Render::InstanceHandle Place_Num = -1;
	Render::InstanceHandle Winner = -1;

	//Menu InstanceHandles
	Render::InstanceHandle Background = -1;
	Render::InstanceHandle TitleCard = -1;
	Render::InstanceHandle Menu_Item_1 = -1;
	Render::InstanceHandle Menu_Item_2 = -1;
	Render::InstanceHandle Menu_Item_3 = -1;
	Render::InstanceHandle Instructions = -1;

	//UI MaterialHandles
	Render::MaterialHandle SI_Bar_png;
	Render::MaterialHandle SI_Critical_png;
	Render::MaterialHandle SI_Damaged_png;
	Render::MaterialHandle SI_Secure_png;
	Render::MaterialHandle First_png;
	Render::MaterialHandle Second_png;
	Render::MaterialHandle Third_png;
	Render::MaterialHandle Fourth_png;
	Render::MaterialHandle UI_Lap_png;
	Render::MaterialHandle UI_Place_png;
	Render::MaterialHandle You_Win_png;
	Render::MaterialHandle P2_Wins_png;
	Render::MaterialHandle P3_Wins_png;
	Render::MaterialHandle P4_Wins_png;

	//Menu MaterialHandles
	// H = hilight, U = Unselected, R = Ready
	Render::MaterialHandle Background_png;
	Render::MaterialHandle Circuitron_Title_png;
	Render::MaterialHandle Exit_H_png;
	Render::MaterialHandle Exit_U_png;
	Render::MaterialHandle Instructions_png;
	Render::MaterialHandle Main_Menu_H_png;
	Render::MaterialHandle Main_Menu_U_png;
	Render::MaterialHandle Options_H_png;
	Render::MaterialHandle Options_U_png;
	Render::MaterialHandle Paused_png;
	Render::MaterialHandle Play_H_png;
	Render::MaterialHandle Play_R_png;
	Render::MaterialHandle Play_U_png;

  public:

	int currentPlace = 4;
	int currentLap = 1;

	UiGame(Render::Render& render, Window& window, bool& menuActive, EntityManager& e_manager, Audio::AudioEngine& stereo)
		: render(render), window(window), menuActive(menuActive), e_manager(e_manager), stereo(stereo), 
		  // Load all images into Materials
		  SI_Bar_png(Render::importUI("assets/UI/SI_Bar.png", render)),
		  SI_Critical_png(Render::importUI("assets/UI/SI_Bar_Critical.png", render)),
		  SI_Damaged_png(Render::importUI("assets/UI/SI_Bar_Damaged.png", render)),
		  SI_Secure_png(Render::importUI("assets/UI/SI_Bar_Secure.png", render)),
		  First_png(Render::importUI("assets/UI/UI_1st.png", render)),
		  Second_png(Render::importUI("assets/UI/UI_2nd.png", render)),
		  Third_png(Render::importUI("assets/UI/UI_3rd.png", render)),
		  Fourth_png(Render::importUI("assets/UI/UI_4th.png", render)),
		  UI_Lap_png(Render::importUI("assets/UI/UI_Lap.png", render)),
		  UI_Place_png(Render::importUI("assets/UI/UI_Place.png", render)),
		  You_Win_png(Render::importUI("assets/UI/You Win!.png", render)),
		  P2_Wins_png(Render::importUI("assets/UI/P2_Wins..png", render)),
		  P3_Wins_png(Render::importUI("assets/UI/P3_Wins..png", render)),
		  P4_Wins_png(Render::importUI("assets/UI/P4_Wins..png", render)),

		  Background_png(Render::importUI("assets/UI/Background.png", render)),
		  Circuitron_Title_png(Render::importUI("assets/UI/CircuiTron Title.png", render)),
		  Exit_H_png(Render::importUI("assets/UI/Exit (hilighted).png", render)),
		  Exit_U_png(Render::importUI("assets/UI/Exit (unselected).png", render)),
		  Instructions_png(Render::importUI("assets/UI/Instructions.png", render)),
		  Main_Menu_H_png(Render::importUI("assets/UI/Main Menu (hilighted).png", render)),
		  Main_Menu_U_png(Render::importUI("assets/UI/Main Menu (unselected).png", render)),
		  Options_H_png(Render::importUI("assets/UI/Options (hilighted).png", render)),
		  Options_U_png(Render::importUI("assets/UI/Options (unselected).png", render)),
		  Paused_png(Render::importUI("assets/UI/Paused.png", render)),
		  Play_H_png(Render::importUI("assets/UI/Play (hilighted).png", render)),
		  Play_R_png(Render::importUI("assets/UI/Play (ready).png", render)),
		  Play_U_png(Render::importUI("assets/UI/Play (unselected).png", render)){};

	void enter() override {
		// You can pass the transform as a 3rd param
		// I set the transform in update so I don't need to set it here.
		//SI_Bar and SI_Fill make up the SI health bar
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
		//Lap_UI and Lap_Num make up the Lap tracker UI
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
		//Place_UI and Place_Num make up the position tracker
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
		//Winner shows who won the race at the end
		if (Winner == -1) {
			auto position = glm::vec3{0, -1080, 0} / 6.0f;
			auto scale = glm::vec2{212, 51} / 2.0f;
			auto depth = 0.0f;
			auto transform = Render::ui_transform(position, scale, depth);

			Winner = render.create_instance(render.ui_mesh(), You_Win_png, transform);
		}
		//The menu background, this is opaque/has no transparency*
		if (Background == -1) {
			auto position = glm::vec3{0, 0, 0} / 4.0f;
			auto scale = glm::vec2{1920, 1080} / 2.0f;
			auto depth = 0.5f;
			auto transform = Render::ui_transform(position, scale, depth);

			Background = render.create_instance(render.ui_mesh(), Background_png, transform);
		}
		//CircuiTron
		if (TitleCard == -1) {
			auto position = glm::vec3{0, 0, 0} / 4.0f;
			auto scale = glm::vec2{1440, 176} / 2.0f;
			auto depth = 1.0f;
			auto transform = Render::ui_transform(position, scale, depth);

			TitleCard = render.create_instance(render.ui_mesh(), Circuitron_Title_png, transform);
		}
		//Menu_Items 1-3 make up the 3 locations for menu buttons to be placed
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
		//Instructions covers the span of the screen during pause/loading
		if (Instructions == -1) {
			auto position = glm::vec3{0, 0, 0} / 4.0f;
			auto scale = glm::vec2{1920, 1080} / 2.0f;
			auto depth = 0.6f;
			auto transform = Render::ui_transform(position, scale, depth);

			Instructions = render.create_instance(render.ui_mesh(), Instructions_png, transform);
			render.instance_set_material(Instructions, -1);
		}
	}
#pragma endregion

	void update(float) override {
	#pragma region UI tutorial
		// All measurements assume a 1920 by 1080 screen
		// This can be adjusted with the global_scale paremeter in ui_transform
		// The default value 2.0f / vec2{1920, -1080} corresponds to a 1920x1080 resolution

		// Position of the center of the element from the center of the screen
		// In this case, if the screen is 1920x1080, the element will be centered on the mouse
		// auto position = glm::vec3{window.cursor.xpos, window.cursor.ypos, 0} - glm::vec3{1920, 1080, 0} / 2.0f;
		// auto position = glm::vec3{1920, 1080, 0} / 4.0f;
		// Half-size of the element
		// In this case the element should be 146x76 so we divide it by 2 to get the half-size
		// auto scale = glm::vec2{146, 76} / 2.0f;

		// (Optional) ranges from -1 to 1. Determines what objects should be on top
		// I don't actually know if -1 or 1 is on top. ¯\_(ツ)_/¯
		// -1 is on the bottom; 1 is on the top
		// auto depth = 0.0f;

		// Radians
		// auto rotation = window.scroll.ypos / 5.0f;
		// auto rotation = 0.f;

		// You can define the transform manually if you're feeling brave
		// but you should probably just use this helper function
		// auto transform = Render::ui_transform(position, scale, depth, rotation);

		// render.instance_set_trans(SI_Bar, transform);
		// render.instance_set_material(SI_Fill, SI_Secure_png);
		// render.instance_set_material(Lap_UI, UI_Lap_png);
		// updatePlace(currentPlace);
		// updateLap(currentPlace);
		// Set the material
		// if (window.mouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		//	render.instance_set_material(instance, play_highlight);
		//} else {
		//	render.instance_set_material(instance, play_ready);
		//}
	#pragma endregion
		//if the paused or menuActive bools have changed, call toggleGameUI
	}

	#pragma region race functions
	void updateLap(int lap) {
		currentLap = lap;
		if (currentLap == 1) {
			render.instance_set_material(Lap_Num, First_png);
		} else if (currentLap == 2) {
			render.instance_set_material(Lap_Num, Second_png);
		} else if (currentLap == 3) {
			render.instance_set_material(Lap_Num, Third_png);
		} else if (currentLap == 4) {
			render.instance_set_material(Lap_Num, Fourth_png);
		}
	}

	void updatePlace(int place) {
		currentPlace = place;
		if (currentPlace == 1) {
			render.instance_set_material(Place_Num, First_png);
		} else if (currentPlace == 2) {
			render.instance_set_material(Place_Num, Second_png);
		} else if (currentPlace == 3) {
			render.instance_set_material(Place_Num, Third_png);
		} else if (currentPlace == 4) {
			render.instance_set_material(Place_Num, Fourth_png);
		}
	}

	void updateSI(float currentHealth) {
		float ratio = currentHealth / 100;
		auto position = glm::vec3{1920 * -0.35, 1080 * 0.45, 0};
		auto scale = glm::vec2{401/1.5f*ratio, 41/1.5f};
		auto depth = 1.0f;
		auto transform = Render::ui_transform(position, scale, depth);
		render.instance_set_trans(SI_Fill, transform);

		if (currentHealth <= 25) {
			render.instance_set_material(SI_Fill, SI_Critical_png);
		} else if (currentHealth >= 76) {
			render.instance_set_material(SI_Fill, SI_Secure_png);
		} else {
			render.instance_set_material(SI_Fill, SI_Damaged_png);
		}
	}

	void winner(int bike) {
		if (bike == -1) {
			render.instance_set_material(Winner, -1);
		} else if (bike == 0) {
			render.instance_set_material(Winner, You_Win_png);
		} else if (bike == 1) {
			render.instance_set_material(Winner, P2_Wins_png);
		} else if (bike == 2) {
			render.instance_set_material(Winner, P3_Wins_png);
		} else if (bike == 3) {
			render.instance_set_material(Winner, P4_Wins_png);
		}
	}
	#pragma endregion

	#pragma region menu functions
	void pause() { 
		paused = true;
		menuActive = true;
		currentlySelectedMenuItem = 1;
		currentlyActiveMenu = 4;
		selectMenuItem(0);

		//UI element toggle
		//render.instance_set_material(Instructions, Instructions_png);

	}

	void unpause() { 
		paused = false;
		menuActive = false;
		currentlyActiveMenu = 0;// 0 is no menu

		//UI element toggle
		//render.instance_set_material(Instructions, -1);

	}

	void play() { 
		paused = false;
		menuActive = false;
		//UI element toggle
	}

	bool getMenuActive() { return menuActive;	}


	//sets/switches menu selection hilight
	//1 for next, -1 for previous, anything else defaults
	void selectMenuItem(int direction) {
		if (direction == 1) {	// switch downwards
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
		//std::cout << currentlySelectedMenuItem << std::endl;

		if (currentlyActiveMenu == 1) { // main menu
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
			render.instance_set_material(Menu_Item_1, Paused_png);
			render.instance_set_material(Menu_Item_2, Play_R_png);
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
	void enterMenuItem();

	void toggleGameUI() {
		//currentlySelectedMenuItem = 1;

		if (currentlyActiveMenu == 1 || currentlyActiveMenu == 2) {//if main menu/options menuActive && !paused
			//toggle off
			render.instance_set_material(SI_Bar, -1);
			render.instance_set_material(SI_Fill, -1);
			render.instance_set_material(Place_Num, -1);
			render.instance_set_material(Place_UI, -1);
			render.instance_set_material(Lap_Num, -1);
			render.instance_set_material(Lap_UI, -1);
			render.instance_set_material(Instructions, -1);
			render.instance_set_material(Winner, -1);
			//toggle on
			render.instance_set_material(Background, Background_png);
			render.instance_set_material(TitleCard, Circuitron_Title_png);
			//render.instance_set_material(Background, -1);
			//render.instance_set_material(TitleCard, -1);
			//render.instance_set_material(Menu_Item_1, -1);
			//render.instance_set_material(Menu_Item_2, -1);
			//render.instance_set_material(Menu_Item_3, -1);
		} else if (currentlyActiveMenu == 3) {//loading screen !menuActive && paused
			//toggle off

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
			//toggle on
			render.instance_set_material(SI_Bar, SI_Bar_png);
			render.instance_set_material(SI_Fill, SI_Secure_png);
			render.instance_set_material(Place_Num, UI_Place_png);
			render.instance_set_material(Place_UI, UI_Place_png);
			render.instance_set_material(Lap_Num, First_png); // might need to make dynamic
			render.instance_set_material(Lap_UI, UI_Lap_png);
			//render.instance_set_material(Background, -1);
			//render.instance_set_material(Menu_Item_1, -1);
			//render.instance_set_material(Instructions, -1);
		} else if (currentlyActiveMenu == 4){//paused menuActive && paused
			// toggle off
			render.instance_set_material(TitleCard, -1);
			render.instance_set_material(Background, -1);
			// toggle on
			//render.instance_set_material(SI_Bar, -1);
			//render.instance_set_material(SI_Fill, -1);
			//render.instance_set_material(Place_Num, -1);
			//render.instance_set_material(Place_UI, -1);
			//render.instance_set_material(Lap_Num, -1);
			//render.instance_set_material(Lap_UI, -1);
			//render.instance_set_material(Winner, -1);
			//render.instance_set_material(Menu_Item_1, -1);
			//render.instance_set_material(Menu_Item_2, -1);
			//render.instance_set_material(Menu_Item_3, -1);
			//render.instance_set_material(Instructions, -1);
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
			//render.instance_set_material(Winner, -1);
		} 
	}

	#pragma endregion
};