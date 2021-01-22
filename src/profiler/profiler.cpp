#include "profiler.hpp"
#include "imgui.h"

Profiler::Profiler() {}

void Profiler::newFrame(std::string section) {
	using namespace std::chrono;

	auto time = high_resolution_clock::now();

	fps = 1 / std::chrono::duration_cast<std::chrono::duration<float>>(time - frameStart).count();

	frameStart = time;
}

void Profiler::section(std::string) {}

void Profiler::draw() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin(
		"Profiler", nullptr,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
	ImGui::End();
}