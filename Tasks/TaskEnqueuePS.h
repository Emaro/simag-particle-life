#pragma once

#include "ITask.h"

class TaskEnqueuePS : public CTask
{
public:

	TaskEnqueuePS() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskEnqueuePS"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	void doEnqueue();

	int m_psSrcIdx = 0;
	int m_psDstIdx = 1;
	bool m_enable = false;
};
