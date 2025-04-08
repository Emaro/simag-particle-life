#pragma once

#include <string>

#include <glm/glm.hpp>

class ParticleSystem;

class ICollision
{
public:

	virtual void handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction, float kt_tangentialFriction) = 0;
	virtual void handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction, float kt_tangentialFriction) = 0;

	virtual glm::vec3 position() = 0;
	virtual void setPosition(glm::vec3 const& pos) = 0;

	virtual void draw() const = 0;

	virtual void imGui(std::string const& pre) = 0;

	virtual const char* toString() const = 0;

	// Needed for Mac
	virtual ~ICollision() = default;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CCollision : public ICollision
{

public:

	virtual void handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override {};
	virtual void handleCollisionByForce(ParticleSystem& ps, float forceStrength=100.0f, float kn_normalFriction = 0.0f, float kt_tangentialFriction = 0.0f) override {};

	virtual glm::vec3 position() override = 0;
	virtual void setPosition(glm::vec3 const& pos) override = 0;

	virtual void draw() const override {};

	virtual void imGui(std::string const& pre) override {};

	virtual const char* toString() const override { return "UNDEFINED"; }

	// ------------------------------------------------------------------------

	CCollision() = default;
	~CCollision() override = default;

protected:

	void handleFriction(glm::vec3& particleVel, glm::vec3& particleForce, glm::vec3& collisionNormal, float kn_normalFriction, float kt_tangentialFriction)
	{
		glm::vec3& v = particleVel;
		glm::vec3& f = particleForce;
		glm::vec3& n = collisionNormal;

		auto vn = n * glm::dot(n, v);
		auto vt = v - vn;

		v  -= vn * kn_normalFriction;
		f  -= vt * kt_tangentialFriction;
	}

};
