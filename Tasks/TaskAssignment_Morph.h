#pragma once

#include "ITask.h"

class TaskAssignment_Morph : public CTask
{
public:

	TaskAssignment_Morph() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskAssignment_Morph"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float m_morphSpeed = 0.1f;

};
