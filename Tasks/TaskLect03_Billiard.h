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
	

	bool useLineSegments = true;

	class Line
	{
	private:
		glm::vec3 p0;
		glm::vec3 p1;
		float rot = 0; // rotation
		glm::vec3 cent; // center

		void swap(glm::vec3& v0, glm::vec3& v1) { glm::vec3 dummy = v0; v0 = v1; v1 = dummy; }

	public:
		Line(glm::vec3 l0, glm::vec3 l1) : p0(l0), p1(l1)
		{
			update();
		}

		void update()
		{
			const float pi = 3.14159265359f;
			rot = atan2(p1.y - p0.y, p1.x - p0.x);
			cent = (p0 + p1) / 2.0f;
		}

		glm::vec3 const& l0() const { return p0; }
		glm::vec3 const& l1() const { return p1; }
		glm::vec3 const& center() const { return cent; }
		float const& rotation() const { return rot; }
		bool isPointWithinLineRange(glm::vec3 const& p) const
		{
			return (p.x >= p0.x && p.x <= p1.x);
		}
		void setLine(glm::vec3 const& l0, glm::vec3 const& l1) { p0 = l0; p1 = l1; update(); }
	};

	std::vector<Line> m_lines;
	void handleBallLineCollision(int i, Line const& line);
	void handleBallLineCollision();
	void copyLinesToPS1();
	void copyLinesFromPS1();
	void createLines0();
	void createLines1();
	void createLines2();
	void drawLineSegments() const;
	void clearLines() { m_lines.clear(); }
};
