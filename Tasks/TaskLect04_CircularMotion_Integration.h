#pragma once

#include "ITask.h"

class TaskLect04_CircularMotion_Integration : public CTask
{
public:

	TaskLect04_CircularMotion_Integration() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskLect04_CircularMotion_Integration"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

};
