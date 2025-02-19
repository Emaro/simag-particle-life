#pragma once

#include "ITask.h"

class TaskAssignment_Fireworks : public CTask
{
public:

	TaskAssignment_Fireworks() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskAssignment_Fireworks"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float m_initSpeedY = 18.0f;
	int m_lifetime = 50;
	int m_createNParticles = 100;
	bool m_useBallistics = false;

	int m_lifetimeCounter = 0;

};
