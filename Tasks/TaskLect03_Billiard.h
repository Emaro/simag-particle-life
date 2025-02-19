#pragma once

#include "ITask.h"

class TaskLect03_Billiard : public CTask
{
public:

	TaskLect03_Billiard() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskLect03_Billiard"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	// Helper class Ball
	// Is connected to particle system
	// But adds radius per particle
	class Ball
	{
	public:
		Ball(ParticleSystem& ps, int particleIdx, glm::vec3 const& pos, glm::vec3 const& vel, float radius, float mass)
			: m_ps(ps), m_idx(particleIdx), m_radius(radius)
		{
			this->pos() = pos;
			this->vel() = vel;
			this->mass() = mass;
		}

		glm::vec3& pos() { return m_ps.particle(m_idx).pos(); }
		const glm::vec3& pos() const { return m_ps.particle(m_idx).pos(); }
		glm::vec3& vel() { return m_ps.particle(m_idx).vel(); }
		float& mass() { return m_ps.particle(m_idx).mass(); }
		float& radius() { return m_radius; }
		const float& radius() const { return m_radius; }
		bool isStatic() const { return m_ps.particle(m_idx).isStatic(); }

	private:
		ParticleSystem& m_ps;
		int m_idx;
		float m_radius;
	};

	std::vector<Ball> m_balls;

	void createScene();
	void createScene2();
	void addBall(glm::vec3 const& pos, glm::vec3 const& vel, float radius, float mass);
	void handleBallBallCollision(int i, int j);
	void handleBallWallCollision(int i);
	void setRandomVelocities();
	void setRandomRadius();

	float m_restitution = 1.0f;
	float m_friction = 0.995f;
	
	glm::vec3 m_wallMin = glm::vec3(-5, -3, 0);
	glm::vec3 m_wallMax = glm::vec3(5, 3, 0);

	// ---
	// The following is only dummy-code, which will be replaces in lecture 04.
	

#ifndef USE_LINES
	bool useLineSegments = false;
	void handleBallLineCollision() {};
	void drawLineSegments() const {};
	void clearLines() {};
	void copyLinesToPS1() {};
	void copyLinesFromPS1() {};
	void createLines0() {};
	void createLines1() {};
	void createLines2() {};

#endif
};
