#pragma once

#include "entities/entity.hpp"
#include "render/import.hpp"
#include "render/ui_util.hpp"

class UiTest : public Entity {
  private:
	Render::Render& render;
	Window& window;
	Render::InstanceHandle instance = -1;

	Render::MaterialHandle play_ready;
	Render::MaterialHandle play_highlight;

  public:
	UiTest(Render::Render& render, Window& window)
		: render(render), window(window), 
		  // Load all images into Materials
		  play_ready(Render::importUI("assets/UI/Play (ready).png", render)),
		  play_highlight(Render::importUI("assets/UI/Play (hilighted).png", render)){};
	void enter() override {
		if (instance == -1) {
			// You can pass the transform as a 3rd param
			// I set the transform in update so I don't need to set it here.
			instance = render.create_instance(render.ui_mesh(), play_ready);
		}
	}

	void update(float) override {
		// All measurements assume a 1920 by 1080 screen
		// This can be adjusted with the global_scale paremeter in ui_transform
		// The default value 2.0f / vec2{1920, -1080} corresponds to a 1920x1080 resolution

		// Position of the center of the element from the center of the screen
		// In this case, if the screen is 1920x1080, the element will be centered on the mouse
		auto position = glm::vec3{window.cursor.xpos, window.cursor.ypos, 0} - glm::vec3{1920, 1080, 0} / 2.0f;
		// Half-size of the element
		// In this case the element should be 146x76 so we divide it by 2 to get the half-size
		auto scale = glm::vec2{146, 76} / 2.0f;

		// (Optional) ranges from -1 to 1. Determines what objects should be on top
		// I don't actually know if -1 or 1 is on top. ¯\_(ツ)_/¯
		auto depth = 0.0f;

		// Radians
		auto rotation = window.scroll.ypos / 5.0f;

		// You can define the transform manually if you're feeling brave
		// but you should probably just use this helper function
		auto transform = Render::ui_transform(position, scale, depth, rotation);

		render.instance_set_trans(instance, transform);

		// Set the material
		if (window.mouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			render.instance_set_material(instance, play_highlight);
		} else {
			render.instance_set_material(instance, play_ready);
		}
	}
};