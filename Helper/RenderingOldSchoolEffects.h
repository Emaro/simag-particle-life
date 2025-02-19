#pragma once

#include <functional>

#include <glm/vec3.hpp>

// Notes
// Stencil buffer must be available
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
// todo:
// set parameters GL_POLYGON_OFFSET_FILL
// ensrue blend mode is like before


class RenderingOldSchoolEffects
{

public:

	static void beginShadowProjection(
		glm::vec3 const& lightPos, glm::vec3 const& planeP, glm::vec3 const& planeN,
		std::function<void()> const& drawFloor,
		std::function<void()> const& drawFloorAlpha,
		std::function<void()> const& drawScene
	);
	static void endShadowProjection();

	static void beginMirror(
		glm::vec3 const& lightPos, glm::vec3 const& planeP, glm::vec3 const& planeN,
		std::function<void()> const& drawFloor,
		std::function<void()> const& drawScene
	);
	static void endMirror() {}



};