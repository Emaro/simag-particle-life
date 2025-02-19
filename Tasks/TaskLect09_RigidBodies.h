#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "ITask.h"

class RigidBody;

class TaskLect09_RigidBodies : public CTask
{
public:

	TaskLect09_RigidBodies() : CTask(), m_initialAngularVelocity(glm::vec3(0,0,4.0f)), m_closestParticlePosOnClick(glm::vec3(0)) {}
	
	// ITask
	virtual const char* name() const override { return "TaskLect09_RigidBodies"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	// parameter
	glm::vec3 m_initialAngularVelocity;
	bool m_rbDrawAxis = true;
	bool m_rbDrawVelocity = true;
	bool m_rbDrawPositionsInit = false;

	float m_interactionForceIntensity = 20.0f;

	float m_gravity = -9.81f;

	// floor collision
	float m_floorCollisionForceScale = 2000.0f;
	float m_floorCollisionForceDamping = 100.0f; // normal damping
	float m_floorCollisionForceFriction = 100.0f; // tangential friction

	std::shared_ptr<RigidBody> rb;
	void generateRigidBody();
	
	// hacky helper for interaction / basically copied from TaskPickParticle.cpp
	int m_closestParticleIdx = -1;
	glm::vec3 m_closestParticlePosOnClick;
	void findClosestParticle();

};
