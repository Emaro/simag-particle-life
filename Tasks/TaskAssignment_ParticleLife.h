#pragma once

#include "ITask.h"

class TaskAssignment_ParticleLife : public CTask
{
public:

	TaskAssignment_ParticleLife() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskAssignment_ParticleLife"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:
	int m_workOnPsIdx = 0;
	int m_noParticles = 1000;
	float m_viscosity = 0.05f;
	float m_repelDist = 1.0f;
	float m_repelForce = 1.0f;

	bool m_bounded = false;
	int m_w_xmin = -5;
	int m_w_xmax = 5;
	int m_w_ymin = -5;
	int m_w_ymax = 5;

	float m_forceTable[5][5];
	float m_radiusTable[5][5];

	// Green interactions
	float m_gxg = -0.0f; // green on green
	float m_gxr = -0.0f; // green on red
	float m_gxb = -0.0f; // green on blue
	float m_gxy = -0.0f; // green on yellow
	float m_gxp = -0.0f; // green on purple
	float m_rgxg = 2.0f; // radius green on green
	float m_rgxr = 2.0f; // radius green on red
	float m_rgxb = 2.0f; // radius green on blue
	float m_rgxy = 2.0f; // radius green on yellow
	float m_rgxp = 2.0f; // radius green on purple

	// Red interactions
	float m_rxg = -0.0f; // red on green
	float m_rxr = -0.0f; // red on red
	float m_rxb = -0.0f; // red on blue
	float m_rxy = -0.0f; // red on yellow
	float m_rxp = -0.0f; // red on purple
	float m_rrxg = 2.0f; // radius red on green
	float m_rrxr = 2.0f; // radius red on red
	float m_rrxb = 2.0f; // radius red on blue
	float m_rrxy = 2.0f; // radius red on yellow
	float m_rrxp = 2.0f; // radius red on purple

	// Blue interactions
	float m_bxg = 0.0f; // blue on green
	float m_bxr = 0.0f; // blue on red
	float m_bxb = 0.0f; // blue on blue
	float m_bxy = 0.0f; // blue on yellow
	float m_bxp = 0.0f; // blue on purple
	float m_rbxg = 2.0f; // radius blue on green
	float m_rbxr = 2.0f; // radius blue on red
	float m_rbxb = 2.0f; // radius blue on blue
	float m_rbxy = 2.0f; // radius blue on yellow
	float m_rbxp = 2.0f; // radius blue on purple

	// Yellow interactions
	float m_yxg = 0.0f; // yellow on green
	float m_yxr = 0.0f; // yellow on red
	float m_yxb = 0.0f; // yellow on blue
	float m_yxy = 0.0f; // yellow on yellow
	float m_yxp = 0.0f; // yellow on purple
	float m_ryxg = 2.0f; // radius yellow on green
	float m_ryxr = 2.0f; // radius yellow on red
	float m_ryxb = 2.0f; // radius yellow on blue
	float m_ryxy = 2.0f; // radius yellow on yellow
	float m_ryxp = 2.0f; // radius yellow on purple

	// Purple interactions
	float m_pxg = 0.0f; // purple on green
	float m_pxr = 0.0f; // purple on red
	float m_pxb = 0.0f; // purple on blue
	float m_pxy = 0.0f; // purple on yellow
	float m_pxp = 0.0f; // purple on purple
	float m_rpxg = 2.0f; // radius purple on green
	float m_rpxr = 2.0f; // radius purple on red
	float m_rpxb = 2.0f; // radius purple on blue
	float m_rpxy = 2.0f; // radius purple on yellow
	float m_rpxp = 2.0f; // radius purple on purple

	// Functions
	void generateRandomScene();
	void fillInteractionTables();
};
