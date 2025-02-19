#pragma once

#include "ITask.h"

class TaskTools : public CTask
{
public:

	TaskTools() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskTools"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	void clearPs();
	void generateCircle(int n);
	void generateQuad();

};
