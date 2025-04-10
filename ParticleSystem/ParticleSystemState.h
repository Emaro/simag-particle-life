#pragma once

#include "ParticleSystem.h"
#include "../Helper/HelperReadPLY.h"

class ParticleSystemState
{
public:

	ParticleSystemState();

	ParticleSystem& particleSystem() { return ps; }
	ParticleSystem const& particleSystem() const { return ps; }

	void update(); // update stream
	void drawPsWithCurrentStyle() const;
	void imGui(std::string const& pre);

	bool simulate() const { return m_bSimulate; }
	void setSimulate(bool simulate) { m_bSimulate = simulate; }

	std::string toString() const; // all info/statistics about content in string

private:

	ParticleSystem ps;

	bool m_bSimulate = true;

	bool m_bDraw = true;
	bool m_bDrawPoints = true;
	bool m_bDrawVelocities = false;
	bool m_bDrawForces = false;
	bool m_bDrawNeighbors = true;

	bool m_bShowPsInfo = false;

	// Color
	bool m_bForceSingleColor = false;
	glm::vec4 m_forceColor = { 0,1,0,1 };

	// 0 none
	// 1 none but load once from file single capture
	// 2 from file loop
	// 3 from file loop range
	// 4 from single capture-file 
	int m_updateStyle = 0;

	// draw
	bool m_bOverrideGlobalDrawSettings = false;
	float m_psPointSize = 2.0f;

	// file loop range from stream
	int m_fileLoopRangeMin = 0;
	int m_fileLoopRangeMax = 1;
	int m_fileLoopRangeActualStart = 0;
	int m_fileLoopRangeActualEnd = 1;
	bool m_bFixRangeMinMax = false; // if fixed, start&end are set equal (to start) => start slider can be used to run through frames

	Helper::ReadFromFilestream m_readFromFilestream;

};
