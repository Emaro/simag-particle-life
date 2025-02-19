#pragma once

#include <string>
#include <algorithm>

#include "../ParticleSystem/ParticleSystemContainer.h"

class ITask
{
public:
	
	// Name of task
	virtual const char* name() const = 0;

	// The following methods are called per frame (!)
	// Order is:
	// 1. imGui()     -> add content to gui / called for *every* rendered frame
	// 2. setForces() -> change forces - called per simuation step / called *only* if simulation is running
	// 3. doWork()    -> anything else, you would like to do / called for *every* rendered frame
	// 4. draw()      -> draw additional content / called for *every* rendered frame

	// Simulation Step
	virtual void stepPre() = 0;
	virtual void setForces() = 0; // Change forces here
	virtual void stepPost() = 0;

	// Do work
	virtual void doWork() = 0;

	// Add some drawings, if needed
	virtual void draw() const = 0;

	// Gui
	virtual void imGui() = 0;

	// Any output -> is shown in task window
	virtual const char* toString() const = 0;

	// Initialization - called, when task is selected in GUI
	virtual void init() = 0;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CTask : public ITask
{
protected:

	int noOfPs() { return m_psc->size(); }
	ParticleSystem& particleSystem(int idx)             { idx = std::clamp<int>(idx, 0, m_psc->size() - 1); return m_psc->particleSystem(idx); }
	ParticleSystem const& particleSystem(int idx) const { idx = std::clamp<int>(idx, 0, m_psc->size() - 1); return m_psc->particleSystem(idx); }

public:

	virtual const char* name() const override = 0;
	virtual void stepPre() override {}; // before setForces
	virtual void setForces() override {}; // called during simulation step
	virtual void stepPost() override {}; // after set Forces
	virtual void doWork() override {};
	virtual void draw() const override {};
	virtual void imGui() override {};
	virtual const char* toString() const override = 0;
	virtual void init() override {};

	// ------------------------------------------------------------------------
	
	CTask() = default;
	virtual ~CTask() = default;
	void setContext(int id, ParticleSystemContainer* psc, std::string const& preParamString) { m_id = id;  m_psc = psc; m_pre = preParamString; }

private:
	std::string m_pre; // string, added in front of parameter names, to avoid param-key-hashmap-conflicts
	mutable std::string m_preConstrTemp;
	int m_id = -1;
	ParticleSystemContainer* m_psc = nullptr;

protected:
	char const* paramName(std::string const& key) const { m_preConstrTemp = m_pre + key; return m_preConstrTemp.c_str(); } // construct unique parameter name
	mutable std::string m_string; // toString() should write into this one & should return the reference

};
