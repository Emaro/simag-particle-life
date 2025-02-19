#pragma once

#include "ITask.h"

#include <glm/glm.hpp>

class TaskLect03_CarCollision : public CTask
{
public:

	TaskLect03_CarCollision()
		: CTask()
		, m_col0(1,1,1,1)
		, m_col1(1,1,1,1)
	{}

	// ITask
	virtual const char* name() const override { return "TaskLect03_CarCollision"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float m_time = 0;

	float m_x0 = -50;
	float m_vel0 = 16.6667f;
	glm::vec4 m_col0;

	float m_x1 = 50;
	float m_vel1 = -14.0f;
	glm::vec4 m_col1;

};
