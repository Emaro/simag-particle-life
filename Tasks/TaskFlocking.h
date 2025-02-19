#pragma once

#include "ITask.h"

class TaskFlocking : public CTask
{
public:

	TaskFlocking() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskFlocking"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	// Boids
	float m_border = 5.0f;
	float m_maxSpeed = 4.0f;
	float m_maxForce = 10.0;

	float m_forceIntensity = 1.0f;

	bool m_force2D = true; // Set z-coordinate to 0.0 per frame

	void handleBorders();
	void handle2D();

	glm::vec3 steerForceCraigReynolds(int particleIdx, glm::vec3 const& direction);

	glm::vec3 separate(int particleIdx);
	float m_desiredSeparation = 0.2f;
	float m_forceSeparation = 1.0f;

	glm::vec3 align(int particleIdx);
	float m_neighborDistAlign = 1.0f;
	float m_forceAlign = 1.0f;

	glm::vec3 cohesion(int particleIdx);
	float m_neighborDistCohesion = 1.0f;
	float m_forceCohesion = 1.0f;
};
