#pragma once

#include "ITask.h"

class TaskLect02_MyFirstTask_Differentiation : public CTask
{
public:

	TaskLect02_MyFirstTask_Differentiation() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskLect02_MyFirstTask_Differentiation"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float myFunc(float x) const;
	float derivative(float x, float h) const;

	// Copy values to particle system
	void copyToParticleSystem0();

	// Setup
	float m_xMin = -5.0f;
	float m_xMax = 5.0f;
	float m_dx = 0.1f;
	float m_h = 0.01f;
	int   m_functionType = 0; // 0-exp, 1-sin, 2-x*x

	bool m_plotDerivative = true;
};
