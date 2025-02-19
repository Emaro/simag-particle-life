#pragma once

struct GLFWwindow;

#include <glm/fwd.hpp>

namespace Helper
{
	// Simple draw routines, for convenience - implemented slow in OpenGl 1.0
	void drawLine2d(float x0, float y0, float x1, float y1);
	void drawLine(glm::vec3 const& x0, glm::vec3 const& x1);
	void drawPoint(glm::vec3 const& p);
	void drawCircle2d(glm::vec3 const& p, float radius, int discretization);
	void drawBox2d(float x0, float y0, float x1, float y1);
	void drawPlane(glm::vec3 const& p, glm::vec3 const& n, bool bDrawNormal = true); // at p, with normal n
	void drawCube(glm::vec3 const& x0, glm::vec3 const& x1);
	void drawSphere(glm::vec3 const& x0, float radius);
	void drawQuad(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3);
	void drawQuad(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3, glm::vec3 const& normal);

	// ------------------------------------------------------------------------

	// Helper
	void drawFloor(float y = 0.0f, float alpha=-1.0f);
	void drawAxis();
	void drawGrid2d();

}