#pragma once

#include <string>

#include "ITask.h"

class TaskMouseAddVelocity : public CTask
{
public:

	TaskMouseAddVelocity() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskMouseAddVelocity"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	glm::vec2 circleCenterFromMouse() const;

	float m_radius=0.5f;
	float m_intensity = 0.1f;
	float m_damping = 0.998f;

	int m_workOnPsIdx = 0;
};
