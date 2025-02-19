#pragma once

#include <glm/glm.hpp>

#include "ITask.h"

class TaskLect05_Springs : public CTask
{
public:

	TaskLect05_Springs() : CTask()	{}

	// ITask
	virtual const char* name() const override { return "TaskLect05_Springs"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	int m_workOnPsIdx = 0;

	float m_kSpring = 0.1f;
	float m_dampingSpring = 0.5f;
	bool  m_bUseDampingSpring = true;

	int m_ropeElementsHalf = 5;
	int m_noOfRopeWithDifferentConnections = 4;
	bool m_createBothEndsStatic = true;

	void generateSpringsAll();

	void generateScene0_pendula();
	void generateScene1_rope();
	void generateScene2_ragdoll();
	void generateScene3_cloth();
	void generateScene4();

	bool isValidParticleIdx(int idx) const;
	void addNeighbor(int idx0, int idx1);
	void addParticle(glm::vec3 const& pos, bool isStatic = false);
	void addBox(glm::vec3 const& p0, glm::vec3 const& p1); // with springs

	bool m_drawSolid = false;

};
