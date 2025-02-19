#pragma once

#include <vector>
#include <string>
#include <assert.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "../Globals.h"
#include "Particle.h"

class ParticleSystem
{

public:

	ParticleSystem();

	void load(std::string const& filename);
	void save(std::string const& filename) const;

	Particle const& particle(int idx) const { assert(size() > 0); return m_states[idx]; }
	Particle& particle(int idx) { assert(size() > 0); return m_states[idx]; }
	Particle& particleLast() { assert(size() > 0); return m_states.back(); }

	size_t size() const { assert(m_pos.size() == m_col.size());  return (int)m_pos.size(); }
	size_t capacity() const { assert(m_pos.size() == m_col.size());  return (int)m_pos.capacity(); }
	void copyFrom(ParticleSystem const& ps);
	void transform(glm::mat4 const& m);
	void projectOnXYAxis();
	void copyPositionsTo(std::vector<glm::vec3>& positions) const;
	int countNaN() const;
	bool isValidIdx(int idx) const { return ((idx >= 0) && (idx < (int)size())); }

	void add(ParticleSystem const& ps);
	void add(glm::vec3 const& pos, glm::vec4 const& color = glm::vec4(1)); // Add single particle
	void add(glm::vec3 const& pos, glm::vec3 const& vel, glm::vec4 const& color) { add(pos, vel, 1.0f, color); } // Add single particle
	void add(glm::vec3 const& pos, glm::vec3 const& vel, float mass = 1.0f, glm::vec4 const& color = glm::vec4(1)); // Add single particle

	void clear();

	void setAllVelocitiesToZero();
	void setAllForcesToZero();
	void setAllVelocitiesAndForcesToZero();
	void setAllMassesToOne();

	// Vector access
	std::vector<glm::vec3> const& positions() const { return m_pos; }
	std::vector<glm::vec3>&       positions() { return m_pos; }
	std::vector<glm::vec3> const& velocities() const { return m_vel; }
	std::vector<glm::vec3>&       velocities() { return m_vel; }
	std::vector<glm::vec3> const& forces() const { return m_forces; }
	std::vector<glm::vec3>&       forces() { return m_forces; }
	std::vector<glm::vec4> const& colors() const { return m_col; }
	std::vector<glm::vec4>&       colors() { return m_col; }
	std::vector<float> const&     mass() const { return m_mass; }
	std::vector<float>&           mass() { return m_mass; }
	std::vector<Particle> const&  states() const { return m_states; }
	std::vector<Particle>&        states() { return m_states; }

#ifdef SIMAG_INCLUDE_MINI_PROJECTS21
#include "../Tasks/MiniProjects21/TaskProject_SlimeMold.inl"
#else
private:

	void resize(size_t n);
#endif

	void reserve(size_t n);

	std::vector<Particle> m_states;
	std::vector<glm::vec3> m_pos;
	std::vector<glm::vec3> m_vel;
	std::vector<glm::vec4> m_col;
	std::vector<glm::vec3> m_forces;
	std::vector<float> m_mass;

	// OpenGL stuff
	unsigned int m_bufferVertex = 0;
	unsigned int m_bufferColor = 0;
	void allocateOpenGlStuff();

	// Temp buffer
	mutable std::vector<glm::vec3> m_temp0;
	mutable std::vector<glm::vec3> m_temp1;
	mutable std::vector<glm::vec3> m_temp2;
	mutable std::vector<glm::vec3> m_temp3;
	void ensureSizeOfTemp() const;

public:

	// You would not need these...

	void clearAllNeighbors();
	void setAllForcesToZeroForStaticParticles();

	// Integration
	void step(int passNumber, float dt);

    // Rendering
	void drawBuffered(void* colorData) const;
	void drawBuffered(glm::vec4 const& color) const;

	void draw() const;
	void draw(float pointSize, glm::vec4 const& color) const;

	void drawSlowLinesPointAndVector(std::vector<glm::vec3> const& pos, std::vector<glm::vec3> const& vec, float scale) const;

	//----------

	void toString(int noOfParticles, std::string& out); // print 1st n particle positions into string
	std::string toString() const;                       // print info/statistics about particlesystem

	//----------
	void set(size_t sizeVertices, uint8_t* pPos, uint8_t* pCol, bool bColorIsChar = false);
	void setVelocities(size_t size, uint8_t* pVel);

	void updateAllParticleStates(); // update m_states according to indices in vectors
	void updateLastParticleState(size_t idx); // update m_states according to indices in vectors
};

// ---------------------------
// ---------------------------
// Inline stuff
// ---------------------------
// ---------------------------

inline glm::vec3 const& Particle::pos() const
{
	return m_ps->positions()[m_idxInPS];
}

inline glm::vec3& Particle::pos()
{
	return m_ps->positions()[m_idxInPS];
}

inline glm::vec3 const& Particle::vel() const
{
	return m_ps->velocities()[m_idxInPS];
}

inline glm::vec3& Particle::vel()
{
	return m_ps->velocities()[m_idxInPS];
}

inline glm::vec3 const& Particle::force() const
{
	return m_ps->forces()[m_idxInPS];
}

inline glm::vec3& Particle::force()
{
	return m_ps->forces()[m_idxInPS];
}

inline glm::vec4 const& Particle::color() const
{
	return m_ps->colors()[m_idxInPS];
}

inline glm::vec4& Particle::color()
{
	return m_ps->colors()[m_idxInPS];
}

inline float const& Particle::mass() const
{
	return m_ps->mass()[m_idxInPS];
}

inline float& Particle::mass()
{
	return m_ps->mass()[m_idxInPS];
}
