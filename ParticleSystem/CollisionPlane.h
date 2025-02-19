#pragma once

#include "ICollision.h"

class CollisionPlane : public CCollision
{

public:

	virtual void handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override;
	virtual void handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override;

	virtual glm::vec3 position() override { return m_p; }
	virtual void setPosition(glm::vec3 const& pos) override { m_p = pos; }

	virtual void draw() const override;

	virtual void imGui(std::string const& pre) override;

	virtual const char* toString() const override;

	// ------------------------------------------------------------------------

	CollisionPlane();
	virtual ~CollisionPlane();


private:

	glm::vec3 m_p; // point on plane
	glm::vec3 m_n; // normal of plane

};