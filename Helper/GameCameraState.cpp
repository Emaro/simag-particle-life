#include "GameCameraState.h"

#include <sstream>

#include "../Context/GlobalEnvironment.h"

void GameCameraState::update(int x, int y)
{
    isCameraMoving = true;
    rotatePosSSold = glm::vec2(x, y);
    rotatePosSSactual = glm::vec2(x, y);
    rotatePosSSprevious = glm::vec2(x, y);
	rotatePosWSold = gEnv->camera->SSToWorld2d(rotatePosSSold);
	
    lookAtDistOld = gEnv->camera->getLookAtDist();
    lookAtDirectionOld = gEnv->camera->getFwd();
	posOld = gEnv->camera->getPos();
	
    upOld = gEnv->camera->getUp();
	scaleOld = scale2d;
}

void GameCameraState::clear()
{
	isCameraMoving = false;
	moveFwd = 0.0f;
	moveRight = 0.0f;
	moveUp = 0.0f;
	speedTranslationFactor = 1.0f;
	speedFactorTranslate = 1.0f;
	rotatePosSSold = glm::vec2(0.0f);
	rotatePosSSactual = glm::vec2(0.0f);
	rotatePosSSprevious = glm::vec2(0.0f);
	lookAtDistOld = 0.0f;
	lookAtDirectionOld = glm::vec3(0.0f);
	upOld = glm::vec3(0, 1, 0);
	posOld = glm::vec3(0, 0, -1);
	dt = 0.0f;
}

void GameCameraState::output() const
{
	std::cerr << toString();
}

std::string GameCameraState::toString() const
{
	std::stringstream ss;

	ss << "GameCameraState\n";
	ss << "dt:                 " << dt << "\n";
	ss << "moveCamera:         " << isCameraMoving << "\n";
	ss << "moveFwd:            " << moveFwd << "\n";
	ss << "moveRight:          " << moveRight << "\n";
	ss << "speedFactorTransl.: " << speedFactorTranslate << "\n";
	ss << "rotatePosSSold:     " << rotatePosSSold.x << "/" << rotatePosSSold.y << "\n";
	ss << "rotatePosSSactual:  " << rotatePosSSactual.x << "/" << rotatePosSSactual.y << "\n";
	ss << "lookAtDistOld:      " << lookAtDistOld << "\n";
	ss << "lookAtDirectionOld: " << lookAtDirectionOld.x << "/" << lookAtDirectionOld.y << "/" << lookAtDirectionOld.z << "\n";

	return ss.str();
}
