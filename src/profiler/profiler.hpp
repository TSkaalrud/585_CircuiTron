#pragma once

#include <chrono>
#include <string>

class Profiler {
  private:
	std::chrono::high_resolution_clock::time_point frameStart;
	float fps;

  public:
	Profiler();
	Profiler(const Profiler&) = delete;

	void newFrame(std::string section);
	void section(std::string section);
	void draw();
};