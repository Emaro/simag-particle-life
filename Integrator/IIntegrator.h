#pragma once

#include <glm/glm.hpp>

class IIntegrator
{
public:
	
	// name of integrator
	virtual const char* name() const = 0;

	// number of passes per specific integrator
	// some integrators need several passes
	// step is called noOfPasses times, while passNumber is increased per call
	// for each step, all forces are recalculated
	virtual int noOfPasses() const = 0;

	// integration
	virtual void step(
		int passNumber,
		float dt,
		glm::vec3* pos,
		glm::vec3* vel,
		glm::vec3* force,
		float* mass,
		glm::vec3* temp0, // store data from pass to pass
		glm::vec3* temp1, // store data from pass to pass
		glm::vec3* temp2, // store data from pass to pass
		glm::vec3* temp3, // store data from pass to pass
		int nSize
	) const = 0;

	// needed for mac & clang
	virtual ~IIntegrator() = default;

};

class CIntegrator : public IIntegrator
{

public:

	virtual const char* name() const override = 0;

	virtual int noOfPasses() const override { return 1; };

	virtual void step(
		int passNumber,
		float dt,
		glm::vec3* pos,
		glm::vec3* vel,
		glm::vec3* force,
		float* mass,
		glm::vec3* temp0,
		glm::vec3* temp1,
		glm::vec3* temp2,
		glm::vec3* temp3,
		int nSize
	) const override = 0;
	// ------------------------------------------------------------------------
	
	CIntegrator() = default;
	virtual ~CIntegrator() = default;

};
