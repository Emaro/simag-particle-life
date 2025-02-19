#pragma once

#include <glm/glm.hpp>

#include "ITask.h"

class TaskAssignment_GravityPlanets : public CTask
{
public:

	TaskAssignment_GravityPlanets() 
		: CTask()
	{}

	// ITask
	virtual const char* name() const override { return "TaskAssignment_GravityPlanets"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	float m_gravityIntensity = 0.1f;

	void generateScene0();
	void generateScene1();
	void generateScene2();

};
