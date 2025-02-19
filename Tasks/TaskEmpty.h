#pragma once

#include "ITask.h"

class TaskEmpty : public CTask
{
public:

	TaskEmpty() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskEmpty"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

};
