#pragma once

#include "IApp.h"
#include "ICallback.h"

#include <memory>

#include "../Tasks/ITask.h"
#include "../Integrator/IIntegrator.h"

#include "../Globals.h"
#include "../ParticleSystem/ParticleSystemContainer.h"

class App : public CApp
{

public:

	// IApp
	virtual void doWork() override;
	virtual void draw() override;
	virtual void imGui() override;
	virtual const char* name() const override { static const char* pAppName = "SimAg"; return pAppName; }

	// ICallbackKey
	virtual void callbackKey(int key, int scancode, int action, int mods) override;

	App();

	// ------------------------------------------------------------------------

	int findTaskIdByName(std::string const& name) const; // returns -1, if not found
	template<class T> void registerTask();
	template<class T> void pushDefaultTask(); // setup configuration of tasks on initialization

private:

	ParticleSystemContainer m_psc;

	// Task List
	static const int cMaxTasks = Globals::MAX_TASKS;
	void createTasks();
	std::vector<std::shared_ptr<ITask> > m_tasks[cMaxTasks];
	std::string m_tasksNamesGuiCombo; // string names of all tasks in one list, i.e. "task1\0task2\0task3\0 ... \0\0"
	int m_currentTask[cMaxTasks];

	// Integrator
	void createIntegrators();
	std::vector < std::shared_ptr<IIntegrator> > m_integrators;
	std::string m_integratorNamesGuiCombo; // string names of all integrators in one list

	// Saving Particle Systems
	Helper::WriteToFilestream m_writeToFilestream;
	bool m_bWriteToFilestream = false;
	int m_savePsFromPSCM_idx = 0;       // idx to PSCM which is used for saving
	ParticleSystem* m_savePs = nullptr; // ps to save/write from

	int m_defaultTaskOnInitCount = 0;
};