#pragma once

#include "ITask.h"

#include "../ParticleSystem/CollisionPlane.h"

class TaskCollisionFloor : public CTask
{
public:

	TaskCollisionFloor() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskCollisionFloor"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	float m_kn_normalFriction = 0.1f;
	float m_kt_tangentialFriction = 0.1f;

	CollisionPlane m_floor;

};
