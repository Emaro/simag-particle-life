#include "TaskLect04_AddForce.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../Helper/HelperDraw.h"

void TaskLect04_AddForce::setForces()
{
    auto& ps = particleSystem(m_workOnPsIdx);

    const glm::vec3 f = m_force;

    float factor = m_divideByNumberOfSteps
        ? (1.0f / (float)gEnv->stateSim->dtFixedNoOfStepsPerFrame)
        : 1.0f;

    const bool bUseGlobalFriction = m_globalFriction > 0.0f;
    const bool bUseArtificialFriction = m_artificialFriction < 1.0f;

    if (m_multiplyWithMass)
    {
        for (auto& it : ps.states())
        {
            it.force() += f * factor * it.mass();
        }
    }
    else
    {
        for (auto& it : ps.states())
        {
            it.force() += f * factor;
        }
    }

    if (bUseArtificialFriction)
    {
        for (auto& it : ps.states())
        {
            it.vel() *= m_artificialFriction;
        }
    }

    if (bUseGlobalFriction)
    {
        for (auto& it : ps.states())
            it.force() -= m_globalFriction * it.vel();
    }

}

void TaskLect04_AddForce::doWork() 
{

}

void TaskLect04_AddForce::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();
    ImGui::SliderFloat3(paramName("Force"), &m_force[0], -10.0f, 10.0f); 
    ImGui::Checkbox(paramName("Treat as acceleration (i.e., multiplyWithMass)"), &m_multiplyWithMass);
    ImGui::Checkbox(paramName("Divide by number of steps!!!"), &m_divideByNumberOfSteps);
    ImGui::Separator();
    ImGui::SliderFloat(paramName("Global friction"), &m_globalFriction, 0.0f, 2.0f);
    ImGui::SliderFloat(paramName("Artificial Friction"), &m_artificialFriction, 0.997f, 1.0f);
}

const char* TaskLect04_AddForce::toString() const
{
    std::stringstream ss;
    ss << "Add constant force" << "\n";
    ss << "Gravity: -9.81 m/s^2" << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskLect04_AddForce::draw() const
{
 
}
