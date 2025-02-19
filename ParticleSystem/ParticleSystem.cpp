#include "ParticleSystem.h"

#include <chrono>
#include <cstring>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Globals.h"
#include "../Helper/HelperReadPLY.h"
#include "../Context/GlobalEnvironment.h"

// ----------------------------------------------------------------------------

namespace
{
	static unsigned int g_StateParticleIdCounter = 0;

	int getUniqueParticleId()
	{
		g_StateParticleIdCounter++;
		return (int)g_StateParticleIdCounter;
	}
}

Particle::Particle(ParticleSystem* ps, size_t idxInPS)
{
	m_id = getUniqueParticleId();
	updatePsIdx(ps, idxInPS);
}

Particle::Particle()
{
	m_id = getUniqueParticleId();
}

void Particle::updatePsIdx(ParticleSystem* ps, size_t idxInPS)
{
	m_idxInPS = idxInPS;
	m_ps = ps;
}

std::string Particle::toString() const
{
	std::stringstream ss;
	ss << "Id:       " << id() << "\n";
	ss << "isStatic: " << isStatic() << "\n";
	ss << "isDead:   " << isDead() << "\n";
	ss << "Mass:     " << mass() << "\n";
	ss << "IdxInPs:  " << m_idxInPS << "\n";
	ss << "Pos:      " << glm::to_string(m_ps->positions()[m_idxInPS]) << "\n";
	ss << "Vel:      " << glm::to_string(m_ps->velocities()[m_idxInPS]) << "\n";
	ss << "Force:    " << glm::to_string(m_ps->forces()[m_idxInPS]) << "\n";
	return ss.str();
}

// ----------------------------------------------------------------------------

ParticleSystem::ParticleSystem()
{
	allocateOpenGlStuff();
}

void ParticleSystem::load(std::string const& filename)
{
	std::string filepath = std::string(Globals::PATH_DATA) + filename + ".ply";
	Helper::readPLY(filepath, this);
	if (size() > 0)
	{
		updateAllParticleStates();
		setAllForcesToZero();
		setAllMassesToOne();
		clearAllNeighbors();
	}

}

void ParticleSystem::save(std::string const& filename) const
{
	if (size() == 0)
	{
		std::cout << "Particle System is empty. Skipping saving file...\n";
		return;
	}
	std::string filepath = std::string(Globals::PATH_DATA) + filename + ".ply";
	Helper::writePLY(filepath, this, &m_vel);
}

void ParticleSystem::resize(size_t n)
{
	m_pos.resize(n);
	m_col.resize(n);
	m_vel.resize(n);
	m_forces.resize(n);
	m_mass.resize(n);
	m_states.resize(n);
}

void ParticleSystem::reserve(size_t n)
{
	m_pos.reserve(n);
	m_col.reserve(n);
	m_vel.reserve(n);
	m_forces.reserve(n);
	m_mass.reserve(n);
	m_states.reserve(n);
}

void ParticleSystem::set(size_t sizeVertices, uint8_t* pPos, uint8_t* pCol, bool bColorIsChar)
{
	if (sizeVertices == 0)
	{
		resize(0);
		return;
	}

	clear();
	resize(sizeVertices);

	std::memcpy(&m_pos.front(), pPos, sizeVertices * 3 * sizeof(float));

	if (pCol != nullptr)
	{
		if (bColorIsChar)
		{
			// is char
			float scale = 1.0f;
			float offs = 0.0f;
			for (size_t i = 0; i < sizeVertices; i++)
			{
				m_col[i].x = (float)pCol[i * 3 + 0] / 255.0f * scale + offs;
				m_col[i].y = (float)pCol[i * 3 + 1] / 255.0f * scale + offs;
				m_col[i].z = (float)pCol[i * 3 + 2] / 255.0f * scale + offs;
				m_col[i].a = 1.0f;
			}
		}
		else
		{
			// is float
			for (size_t i = 0; i < sizeVertices; i++)
			{
				m_col[i].x = ((float*)pCol)[i * 3 + 0];
				m_col[i].y = ((float*)pCol)[i * 3 + 1];
				m_col[i].z = ((float*)pCol)[i * 3 + 2];
				m_col[i].a = 1.0f;
			}
		}
	}
	else
	{
		glm::vec4 colDefault(1, 1, 1, 1);
		for (size_t i = 0; i < m_col.size(); i++)
		{
			m_col[i] = colDefault;
		}
	}
}

void ParticleSystem::setVelocities(size_t size, uint8_t* pVel)
{
	if (size != this->size()) return;
	m_vel.resize(size);
	std::memcpy(&m_vel.front(), pVel, size * 3 * sizeof(float));
}


void ParticleSystem::updateAllParticleStates()
{
	int idx = 0;
	for (auto& it : m_states)
	{
		it.updatePsIdx(const_cast<ParticleSystem*>(this), idx);
		idx++;
	}
}

void ParticleSystem::updateLastParticleState(size_t idx)
{
	m_states.back().updatePsIdx(const_cast<ParticleSystem*>(this), idx);
}

void ParticleSystem::copyFrom(ParticleSystem const& ps)
{
	resize(ps.size());
	if (ps.size() > 0)
	{
		std::memcpy(&m_pos.front(), &ps.positions().front(), ps.size() * 3UL * sizeof(float));
		std::memcpy(&m_col.front(), &ps.colors().front(), ps.size() * 4UL * sizeof(float));
		std::memcpy(&m_vel.front(), &ps.velocities().front(), ps.size() * 3UL * sizeof(float));
		std::memcpy(&m_forces.front(), &ps.forces().front(), ps.size() * 3UL * sizeof(float));
		std::memcpy(&m_mass.front(), &ps.mass().front(), ps.size() * sizeof(float));
		
		m_states.resize(0);
		for (auto& it : ps.states())
		{
			m_states.emplace_back(it);
		}
		updateAllParticleStates();
	}
}

void ParticleSystem::add(ParticleSystem const& ps)
{
	if (capacity() < size() + ps.size())
	{
		reserve(std::max<size_t>(size() * 2, size() + ps.size()));
	}

	auto& p = ps.positions();
	for (auto& it : p) { m_pos.emplace_back(it); }
	auto& c = ps.colors();
	for (auto& it : c) { m_col.emplace_back(it); }
	auto& v = ps.velocities();
	for (auto& it : v) { m_vel.emplace_back(it); }
	auto& f = ps.forces();
	//for (auto& it : f) { m_forces.push_back(it); }
	for (auto& it : f) { m_forces.emplace_back(glm::vec3(0)); }
	auto& m = ps.mass();
	for (auto& it : m) { m_mass.emplace_back(it); }

	auto& s = ps.states();
	for (auto& it : s)
	{
		m_states.emplace_back(Particle(this, m_states.size()));
		//updateLastParticleState();
	}
}

void ParticleSystem::add(glm::vec3 const& pos, glm::vec4 const& color)
{
	m_pos.push_back(pos);
	m_col.push_back(color);
	m_vel.emplace_back(glm::vec3(0));
	m_forces.emplace_back(glm::vec3(0));
	m_mass.push_back(1.0f);

	m_states.emplace_back(Particle(this, size()-1));
	//updateLastParticleState(size() - 1);
}

void ParticleSystem::add(glm::vec3 const& pos, glm::vec3 const& vel, float mass, glm::vec4 const& color)
{
	m_pos.push_back(pos);
	m_vel.push_back(vel);
	m_mass.push_back(mass);
	m_col.push_back(color);
	m_forces.emplace_back(glm::vec3(0));
	m_states.emplace_back(Particle(this, size() - 1));
}

void ParticleSystem::transform(glm::mat4 const& m)
{
	for (auto& it : m_pos)
	{
		glm::vec4 v(it, 1);
		v = m * v;
		it = glm::vec3(v) / v.w;
	}
}

int ParticleSystem::countNaN() const
{
	int noOfNaNPos = 0;

	for (auto it : m_pos)
	{
		float myDot = glm::dot(it, it);
		if (std::isnan(myDot) || std::isinf(myDot))
		{
			noOfNaNPos++;
		}
	}

	return noOfNaNPos;
}

void ParticleSystem::projectOnXYAxis()
{
	for (auto& it : m_pos)
	{
		it.z = 0.0f;
	}
}

void ParticleSystem::clear()
{
	m_pos.clear();
	m_col.clear();
	m_vel.clear();
	m_forces.clear();
	m_mass.clear();
	m_states.clear();
}

void ParticleSystem::setAllVelocitiesToZero()
{
	if (size() == 0) return;
	memset(&m_vel[0], 0, m_vel.size() * sizeof(glm::vec3));
}

void ParticleSystem::setAllForcesToZero()
{
	if (size() == 0) return;
	memset(&m_forces[0], 0, m_forces.size() * sizeof(glm::vec3));
}

void ParticleSystem::setAllForcesToZeroForStaticParticles()
{
	if (size() == 0) return;
	
	for (auto& it : m_states)
	{
		if (it.isStatic())
		{
			auto& f = it.force();
			f.x = 0;
			f.y = 0;
			f.z = 0;

			auto& v = it.vel();
			v.x = 0;
			v.y = 0;
			v.z = 0;
		}
	}
}

void ParticleSystem::setAllVelocitiesAndForcesToZero()
{
	if (size() == 0) return;
	setAllVelocitiesToZero();
	setAllForcesToZero();
}

void ParticleSystem::setAllMassesToOne()
{
	if (size() == 0) return;
	for (auto& it : m_mass)
	{
		it = 1.0f;
	}
}

void ParticleSystem::copyPositionsTo(std::vector<glm::vec3>& positions) const
{
	positions.resize(m_pos.size());
	if (size() > 0)
	{
		memcpy(&positions[0], &m_pos[0], m_pos.size() * sizeof(glm::vec3));
	}
}

void ParticleSystem::toString(int noOfParticles, std::string& out)
{
	size_t n = std::min<size_t>(size(), (size_t)noOfParticles);
	std::stringstream ss;
	for (size_t i = 0; i < n; i++)
	{
		ss << glm::to_string(m_pos[i]) << "\n";
	}
	out = ss.str();
}

std::string ParticleSystem::toString() const
{
	std::stringstream ss;
	ss << "size:       " << size() << "\n";
	ss << "countNaN:   " << countNaN() << "\n";
	ss << "---\n";
	ss << "pos size:      " << m_pos.size() << "\n";
	ss << "col size:      " << m_col.size() << "\n";
	ss << "vel size:      " << m_vel.size() << "\n";
	ss << "forces size:   " << m_forces.size() << "\n";
	ss << "mass size:     " << m_mass.size() << "\n";
	ss << "states size:   " << m_states.size() << "\n";
	//ss << "normals size:  " << m_normals.size() << "\n";
	//ss << "noOfCluster:   " << noOfCluster << "\n";
	//ss << "idCluster size:" << idCluster.size() << "\n";

	return ss.str();
}

void ParticleSystem::ensureSizeOfTemp() const
{
	if (m_temp0.size() != (size_t)size())
	{
		m_temp0.resize(size());
	}
	if (m_temp1.size() != (size_t)size())
	{
		m_temp1.resize(size());
	}
	if (m_temp2.size() != (size_t)size())
	{
		m_temp2.resize(size());
	}
	if (m_temp3.size() != (size_t)size())
	{
		m_temp3.resize(size());
	}
}

void ParticleSystem::clearAllNeighbors()
{
	for (auto& it : m_states)
	{
		it.neighborList().clear();
	}
}


// ----------------------------------------------------------------------------

void ParticleSystem::step(int passNumber, float dt)
{
	if (size() == 0) return;

	ensureSizeOfTemp();

	gEnv->stateSim->integrator->step(
		passNumber,
		dt,
		&m_pos.front(),
		&m_vel.front(),
		&m_forces.front(),
		&m_mass.front(),
		&m_temp0.front(),
		&m_temp1.front(),
		&m_temp2.front(),
		&m_temp3.front(),
		(int)size()
	);
}
