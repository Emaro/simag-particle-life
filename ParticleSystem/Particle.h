#pragma once

#include <vector>
#include <string>
#include <assert.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "NeighborList.h"

class ParticleSystem;

class Particle
{

public:

	unsigned int id() const { return m_id; }

	bool isDead() const { return m_state & (1UL << IS_DEAD); }
	void setDead(bool value = true) { setState(IS_DEAD, value); }
	void kill() { m_state &= ~(1UL << IS_DEAD); }

	bool isStatic() const { return m_state & (1UL << IS_STATIC); }
	void setStatic(bool value = true) { setState(IS_STATIC, value); }

	glm::vec3 const& pos() const;
	glm::vec3& pos();
	glm::vec3 const& vel() const;
	glm::vec3& vel();
	glm::vec3 const& force() const;
	glm::vec3& force();
	glm::vec4 const& color() const;
	glm::vec4& color();
	float const& mass() const;
	float& mass();

	NeighborList const& neighborList() const { return m_neighborList; }
	NeighborList& neighborList() { return m_neighborList; }

private:

	enum eState
	{
		IS_STATIC = 0,
		IS_DEAD = 1,
		eState_LAST = 2
	};

	inline void setState(int const& state, bool const& value) { m_state ^= (-(value ? 1 : 0) ^ m_state) & (1UL << state); }

	NeighborList m_neighborList;

	unsigned int m_id = 0; // global unique id
	unsigned int m_state = 0;

	// link to ps
	ParticleSystem* m_ps = nullptr;
	size_t m_idxInPS = (size_t)-1;

public:

	// You would not need these...

	Particle(ParticleSystem* ps, size_t idxInPS);
	Particle();

	void updatePsIdx(ParticleSystem* ps, size_t idxInPS);

	std::string toString() const;
};

