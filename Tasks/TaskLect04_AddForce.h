#pragma once

#include <glm/glm.hpp>

#include "ITask.h"

class TaskLect04_AddForce : public CTask
{
public:

	TaskLect04_AddForce() 
		: CTask()
		, m_force(0.0f, -9.81f, 0.0f)
	{}

	// ITask
	virtual const char* name() const override { return "TaskLect04_AddForce"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	glm::vec3 m_force;
	bool m_multiplyWithMass = true;
	bool m_divideByNumberOfSteps = false; // divide acting force per step by "number of steps per frame"
	float m_globalFriction = 0.0f; // stokes friction, e.g. air
	float m_artificialFriction = 1.0f; // artificial friction, i.e. multiply velocity by this factor per step

};
