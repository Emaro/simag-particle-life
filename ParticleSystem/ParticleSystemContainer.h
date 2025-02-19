#pragma once

#include <string>
#include <algorithm>

#include "../Globals.h"
#include "ParticleSystemState.h"

class ParticleSystemContainer
{

public:

	ParticleSystemContainer();

	ParticleSystem& particleSystem(int idx) { return m_psStates[getValidIdx(idx)].particleSystem(); }
	ParticleSystem const& particleSystem(int idx) const { return m_psStates[getValidIdx(idx)].particleSystem(); }

	void step(int passNumber, float dt); // integration

	int size() const { return cMaxNoOfPS; }
	void setAllForcesToZero(); // only, if ParticleSystemState::simulate = true
	void setAllForcesToZeroForStaticParticles(); // only, if ParticleSystemState::simulate = true
	std::string toString() const; // all info/statistics about content in string

	void update(); // update stream
	void draw() const;
	void imGui();

private:

	static const int cMaxNoOfPS = Globals::MAX_PARTICLE_SYSTEMS;
	std::vector<ParticleSystemState> m_psStates;

	int getValidIdx(int idx) const { if (idx < 0) return 0; return std::min<int>(idx, cMaxNoOfPS - 1); }
};

