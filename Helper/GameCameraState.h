#pragma once

#include <iostream>

#include <glm/glm.hpp>

struct GameCameraState
{
	bool isCameraMoving; // true, when camera is in move mode

	float moveFwd;
	float moveRight;
	float moveUp;
	float speedFactorTranslate; // on key <shift>
	float scale2d = 0.2f; // transformation scale in 2d

	glm::vec2 rotatePosSSold;                // initial SS pos, e.g., switch to isCameraMoving/mouse-right-click
	glm::vec2 rotatePosWSold;
	glm::vec2 rotatePosSSactual;             // current SS pos
	mutable glm::vec2 rotatePosSSprevious;   // previous frame pos

	// old camera states
	glm::vec3 lookAtDirectionOld;
	float lookAtDistOld;
	glm::vec3 upOld;
	glm::vec3 posOld;
	float scaleOld;

	float dt;                     // needed for time-independent movement
	float speedTranslationFactor; // general speed factor
	float speedRotation;          // default speed constant
	float speedTranslation;       // default speed constant

	GameCameraState(float speedTranslation, float speedRotation)
		: speedRotation(speedRotation)
		, speedTranslation(speedTranslation)
	{
		clear();
	}

	void update(int x, int y);
	void clear();
	void output() const;
	std::string toString() const;
};