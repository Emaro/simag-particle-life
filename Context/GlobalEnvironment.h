#pragma once

#include <memory>

#include "States.h"
#include "../Helper/Camera.h"

struct GlobalEnvironment
{
	StateSimulation* stateSim = nullptr;
	StateGui* stateGui = nullptr;
	Camera* camera = nullptr;

	bool logDebug = false;

	// ---------------------------

	GlobalEnvironment();
	~GlobalEnvironment();
	
	static void init();

	void createCameras(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow);
	void updateCamera(float dt);
	void switchCamera();
};

extern GlobalEnvironment* gEnv;
