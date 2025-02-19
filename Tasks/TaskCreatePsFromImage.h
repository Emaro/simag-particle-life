#pragma once

#include <string>

#include "ITask.h"

class TaskCreatePsFromImage : public CTask
{
public:

	TaskCreatePsFromImage() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskCreatePsFromImage"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	int m_createPos = 1; // 0 - centered on floor, 1 - centered around origin

	float m_scale = 5.0f;

	void createPsFromImage(std::string const& filename);
};
