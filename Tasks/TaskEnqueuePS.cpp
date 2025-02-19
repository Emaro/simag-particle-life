#include "TaskEnqueuePS.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include "../Context/GlobalEnvironment.h"

void TaskEnqueuePS::doEnqueue()
{
    if (m_psSrcIdx == m_psDstIdx) return;

    auto& psSrc = particleSystem(m_psSrcIdx);
    auto& psDst = particleSystem(m_psDstIdx);

    if (&psSrc == &psDst) return;

    psDst.add(psSrc);
}


void TaskEnqueuePS::setForces()
{

}

void TaskEnqueuePS::doWork() 
{
    if (!m_enable) return;
    if (!gEnv->stateSim->isRunning) return;

    doEnqueue();
}

void TaskEnqueuePS::imGui()
{
    ImGui::Checkbox(paramName("Enable for each step, if simulation is running"), &m_enable);
    
    ImGui::SliderInt(paramName("PS Src"), &m_psSrcIdx, 0, noOfPs() - 1);
    ImGui::SliderInt(paramName("PS Dst"), &m_psDstIdx, 0, noOfPs() - 1);

    ImGui::Separator();
    if (ImGui::Button("Enqueue once"))
    {
        doEnqueue();
    }
}

const char* TaskEnqueuePS::toString() const
{
    std::stringstream ss;
    ss << "Description:" << "\n";
    ss << "Adds whole ParticleSystem from Src into Dst." << "\n";
    ss << "Thus all particles from Src are appended into ParticleSystem Dst." << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskEnqueuePS::draw() const
{
}
