#pragma once

#include <string>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "ICallback.h"

class ContextOpenGl
{

public:

	ContextOpenGl(const char* titleWindow="MyWindow");
	virtual ~ContextOpenGl();

	// Next Frame, returns false on exit
	bool doFrame();

	// Helper
	static double getTime() ;
	GLFWwindow* window() const { return m_window; }
	glm::ivec2 const& sizeWindow() const { return m_sizeWindow; }
	glm::ivec2 const& sizeFramebuffer() const { return m_sizeFramebuffer; }
	void idleForTargetFps(double targetFPS);

	void setWindowName(std::string const& name);

	// Additional callbacks
	void setCallbackAdditionalKey(ICallbackKey* ick) { m_callbackAdditionalKey = ick; }

private:

	GLFWwindow* m_window = nullptr;
	GLFWmonitor* m_monitor = nullptr;

	// On Mac framebuffer size (in pixel) and window size (screen coordinates) might differ...
	// I.e., framebuffer might be e.g., twice as large as window size
	glm::ivec2 m_sizeFramebuffer;
	glm::ivec2 m_sizeWindow;
	void setResolution(int widthFramebuffer, int heightFramebuffer, int widthWindow, int heightWindow);

	// Callbacks
	void callbackResize(GLFWwindow* window, int x, int y);
	void callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void callbackChar(GLFWwindow* window, unsigned int c);
	void callbackMouseButton(GLFWwindow* window, int button, int action, int mods);
	void callbackCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	void callbackScroll(GLFWwindow* window, double xoffset, double yoffset);

	// Additional callbacks
	ICallbackKey* m_callbackAdditionalKey = nullptr;

	// State for idleForTargetFps()
	double m_lastTime = -1;
};
