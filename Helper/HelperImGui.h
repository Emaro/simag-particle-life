#pragma once

#include <string>

struct GLFWwindow;

namespace Helper
{
	void ImGuiInit(GLFWwindow* window);
	void ImGuiExit();
	void ImGuiFrameStart();
	void ImGuiFrameEnd();
	void ImGuiShowSimpleWindow(bool&);

	void MySliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.5f", int flags = 0);

	// used several times
	void ImGuiParticleSizeParameter(std::string const& pre, float& particleSize);
	void ImGuiParticleRenderStyleParameter(std::string const& pre, int& particleRenderStyle);
}