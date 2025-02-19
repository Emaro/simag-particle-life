#pragma once

#include "ITask.h"

class TaskLect04_Pendula : public CTask
{
public:

	TaskLect04_Pendula() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskLect04_Pendula"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float m_pendulaLength = 1.0f;
	int   m_noOfPendulas = 2;

	float m_lineWidth = 2.0f;

	// PBD
	std::vector<glm::vec3> posPrev;
};
