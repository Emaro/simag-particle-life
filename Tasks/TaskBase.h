#pragma once

#include "ITask.h"

class TaskBase : public CTask
{
public:

	TaskBase() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskBase"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	bool m_boolTest = false;
	float m_sliderValue = 0.5f;
	float m_count = 0.0f;

};
