#include "GlobalEnvironment.h"

#include "States.h"
#include "../Helper/Camera.h"

GlobalEnvironment* gEnv = nullptr;

GlobalEnvironment::GlobalEnvironment()
{
	stateSim = new StateSimulation;
	stateGui = new StateGui;
}

GlobalEnvironment::~GlobalEnvironment()
{
	delete stateSim;
	delete stateGui;
	if (gEnv) { delete gEnv; }
}

void GlobalEnvironment::init()
{
	gEnv = new GlobalEnvironment;
}

void GlobalEnvironment::createCameras(glm::ivec2 const sizeFramebuffer, glm::ivec2 const sizeWindow)
{
	stateGui->camera2d = Camera::createCamera2d(sizeFramebuffer, sizeWindow);
	stateGui->camera2d->setScale2d(stateGui->cameraState.scale2d);
	stateGui->camera2d->updateView();

	stateGui->camera3d = Camera::createCamera3d(sizeFramebuffer, sizeWindow);
	stateGui->camera3d->setScale2d(stateGui->cameraState.scale2d);
	stateGui->camera3d->updateView();

	camera = stateGui->camera2d.get();
}

void GlobalEnvironment::switchCamera()
{
	if (camera == stateGui->camera2d.get())
	{
		camera = stateGui->camera3d.get();
	}
	else
	{
		camera = stateGui->camera2d.get();
	}

	if (camera)
	{
		camera->updateView(true);
	}
}

void GlobalEnvironment::updateCamera(float dt)
{
	if (stateGui->cameraState.isCameraMoving)
	{
		// Update camera, if rotated about a certain degree
		const float dotAngle = 0.7f;
		if (glm::dot(camera->getFwd(), glm::normalize(stateGui->cameraState.lookAtDirectionOld)) < dotAngle)
		{
			if (logDebug)
			{
				std::cerr << "UPDATE CAMERA\n";
			}
			stateGui->cameraState.update((int)stateGui->mousePos.x, (int)stateGui->mousePos.y);
		}

		//cameraState.output();
		stateGui->cameraState.dt = dt;
		stateGui->cameraState.speedTranslationFactor = stateGui->cameraStateSpeedTranslationFactor;
		camera->move(stateGui->cameraState);
	}
}
