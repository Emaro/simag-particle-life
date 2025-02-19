#pragma once

#include "ITask.h"

#include "../ParticleSystem/CollisionPlane.h"
#include "../ParticleSystem/CollisionCube.h"
#include "../ParticleSystem/CollisionSphere.h"

class TaskCollision : public CTask
{
public:

	TaskCollision() : CTask() { m_collider = &m_colliderSphere; }

	// ITask
	virtual const char* name() const override { return "TaskCollision"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	CollisionPlane  m_colliderPlane;
	CollisionSphere m_colliderSphere;
	CollisionCube   m_colliderCube;
	ICollision* m_collider = nullptr;

	float m_kn_normalFriction = 0.0f;
	float m_kt_tangentialFriction = 0.0f;

	bool  m_handleCollisionByForce = false;
	float m_forceStrength = 100.0f;

	bool m_animateCollider = false;
	float m_animateColliderSpeed = 0.025f;

	int m_colliderType = 1;
	void setCollider() 
	{
		switch (m_colliderType)
		{
		case 0: m_collider = &m_colliderPlane; break;
		case 1: m_collider = &m_colliderSphere; break;
		case 2: m_collider = &m_colliderCube; break;
		}
	}

};
