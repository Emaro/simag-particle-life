#pragma once

#include "ITask.h"

class TaskAssignment_ParticleLife : public CTask
{
public:

	TaskAssignment_ParticleLife() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskAssignment_ParticleLife"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

    // Add private members

	bool m_boolTest = false;
	float m_sliderValue = 0.5f;
	float m_count = 0.0f;
};
