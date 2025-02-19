#pragma once

#include "ICollision.h"

class CollisionCube : public CCollision
{

public:

	virtual void handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override;
	virtual void handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override;

	virtual glm::vec3 position() override { return (m_p0 + m_p1) * 0.5f;; }
	virtual void setPosition(glm::vec3 const& pos) override { glm::vec3 dx = m_p1 - m_p0; m_p0 = pos-dx*0.5f; m_p1 = pos + dx*0.5f; }

	virtual void draw() const override;

	virtual void imGui(std::string const& pre) override;

	virtual const char* toString() const override;

	// ------------------------------------------------------------------------

	CollisionCube();
	virtual ~CollisionCube();


private:

	glm::vec3 m_p0; // posMin
	glm::vec3 m_p1; // posMax

};