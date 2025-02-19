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

void GlobalEnvironment::createCameras(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow)
{
	gEnv->stateGui->camera2d = Camera::createCamera2d(sizeFramebuffer, sizeWindow);
	gEnv->stateGui->camera2d->setScale2d(gEnv->stateGui->cameraState.scale2d);
	gEnv->stateGui->camera2d->updateView();

	gEnv->stateGui->camera3d = Camera::createCamera3d(sizeFramebuffer, sizeWindow);
	gEnv->stateGui->camera3d->setScale2d(gEnv->stateGui->cameraState.scale2d);
	gEnv->stateGui->camera3d->updateView();

	gEnv->camera = gEnv->stateGui->camera2d.get();
}

void GlobalEnvironment::switchCamera()
{
	if (gEnv->camera == gEnv->stateGui->camera2d.get())
	{
		gEnv->camera = gEnv->stateGui->camera3d.get();
	}
	else
	{
		gEnv->camera = gEnv->stateGui->camera2d.get();
	}

	if (gEnv->camera)
	{
		gEnv->camera->updateView(true);
	}
}

void GlobalEnvironment::updateCamera(float dt)
{
	if (gEnv->stateGui->cameraState.isCameraMoving)
	{
		// Update camera, if rotated about a certain degree
		const float dotAngle = 0.7f;
		if (glm::dot(gEnv->camera->getFwd(), glm::normalize(gEnv->stateGui->cameraState.lookAtDirectionOld)) < dotAngle)
		{
			if (logDebug)
			{
				std::cerr << "UPDATE CAMERA\n";
			}
			gEnv->stateGui->cameraState.update((int)gEnv->stateGui->mousePos.x, (int)gEnv->stateGui->mousePos.y);
		}

		//cameraState.output();
		gEnv->stateGui->cameraState.dt = dt;
		gEnv->stateGui->cameraState.speedTranslationFactor = gEnv->stateGui->cameraStateSpeedTranslationFactor;
		gEnv->camera->move(gEnv->stateGui->cameraState);
	}
}
