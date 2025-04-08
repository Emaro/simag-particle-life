#include "TaskTools.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include "../ParticleSystem/ParticleSystemContainer.h"
#include "../Context/GlobalEnvironment.h"

namespace
{
    void setVelocitiesRadial(ParticleSystem& ps)
    {
        for (auto& it : ps.states())
        {
            it.vel() = it.pos();
        }
    }

    void setVelocitiesRadialNormalized(ParticleSystem& ps)
    {
        for (auto& it : ps.states())
        {
            it.vel() = glm::normalize(it.pos());
        }
    }

    void setVelocitiesPerpendicular(ParticleSystem& ps)
    {
        for (auto& it : ps.states())
        {
            it.vel() = glm::vec3(-it.pos().y, it.pos().x, it.pos().z);
        }
    }

    void setVelocitiesPerpendicularNormalized(ParticleSystem& ps)
    {
        for (auto& it : ps.states())
        {
            auto n = glm::normalize(it.pos());
            it.vel() = glm::vec3(-n.y, n.x, n.z);
        }
    }

    void invertVelocities(ParticleSystem& ps)
    {
        for (auto& it : ps.states())
        {
            it.vel() = -it.vel();
        }
    }

}

void TaskTools::clearPs()
{
    particleSystem(m_workOnPsIdx).clear();
}

void TaskTools::generateCircle(int n)
{
    auto& ps = particleSystem(m_workOnPsIdx);

    float radius = 2.0f;
    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 p(radius, 0, 0);

    // Generate n particles on a circle around the root.
    for (int i = 0; i < n; i++)
    {
        float angle = i * glm::two_pi<float>() / n;
        glm::mat2x2 rot(glm::cos(angle), glm::sin(angle), -glm::sin(angle), glm::cos(angle));
        glm::mat3x3 rot3(rot);
        ps.add(rot3 * p, color);
    }
}

void TaskTools::generateQuad()
{
    auto& ps = particleSystem(m_workOnPsIdx);

    float dx = 0.1f;
    glm::vec3 pMin(-1.0f, -1.0f, 0);
    glm::vec3 pMax(1.0f, 1.0f, 0);

    glm::vec4 color(1.0f, 1.0f, 1.0f, 1.0f);

    // Generate a quad of particles with distance dx within pMin and pMax.
    for (float i = pMin.x; i - pMax.x < 1e-5; i += dx)
    for (float j = pMin.y; j - pMax.y < 1e-5; j += dx)
        ps.add(glm::vec3(i, j, 0), color);
}

void TaskTools::setForces()
{

}

void TaskTools::doWork() 
{

}

void TaskTools::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs()-1);

    ImGui::Separator();
    if (ImGui::Button(paramName("Clear PS"))) { clearPs(); }

    ImGui::Separator();
    if (ImGui::Button(paramName("Generate Circle Hi-Res"))) { generateCircle(100); }
    if (ImGui::Button(paramName("Generate Circle Low-Res"))) { generateCircle(10); }
    if (ImGui::Button(paramName("Generate Quad"))) { generateQuad(); }

    ImGui::Separator();
    if (ImGui::Button(paramName("Set Velocities Radial"))) { setVelocitiesRadial(particleSystem(m_workOnPsIdx)); }
    if (ImGui::Button(paramName("Set Velocities Radial Normalized"))) { setVelocitiesRadialNormalized(particleSystem(m_workOnPsIdx)); }
    if (ImGui::Button(paramName("Set Velocities Perpendicular"))) { setVelocitiesPerpendicular(particleSystem(m_workOnPsIdx)); }
    if (ImGui::Button(paramName("Set Velocities Perpendicular Normalized"))) { setVelocitiesPerpendicularNormalized(particleSystem(m_workOnPsIdx)); }
    if (ImGui::Button(paramName("Invert Velocities"))) { invertVelocities(particleSystem(m_workOnPsIdx)); }
}

const char* TaskTools::toString() const
{
    std::stringstream ss;
    ss << "Tools" << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskTools::draw() const
{
}
