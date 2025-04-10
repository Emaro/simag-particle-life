#include "TaskAssignment_GravityPlanets.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include "../Context/GlobalEnvironment.h"

#include "../Helper/HelperDraw.h"

namespace
{

    inline void determineGravityNormalized(
        glm::vec3& outGravity,
        glm::vec3 const& p0,
        float const& mass0,
        glm::vec3 const& p1,
        float const& mass1,
        glm::vec3& temp
    )
    {

        // todo students
        
    }

}

void TaskAssignment_GravityPlanets::setForces()
{
    auto& ps = particleSystem(m_workOnPsIdx);

    int s = (int)ps.size();

    if (s < 1) return;

    auto& pos = ps.positions();
    auto& mass = ps.mass();
    auto& forces = ps.forces();

    glm::vec3 temp;
    glm::vec3 gravity;

    const float intensity = m_gravityIntensity;

    for (int i = 0; i < s; i++)
    for (int j = i+1; j < s; j++)
    {
        temp = pos[j] - pos[i];
        float len = glm::length(temp);
        gravity = intensity * glm::normalize(temp) * mass[i] * mass[j] / (len * len);
        forces[i] += gravity;
        forces[j] -= gravity;
    }

}

void TaskAssignment_GravityPlanets::doWork() 
{

}

void TaskAssignment_GravityPlanets::generateScene0()
{
    gEnv->stateSim->dtFixed = 0.001f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 10;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    ps.add(glm::vec3(0), glm::vec3(0), 100);
    ps.add(glm::vec3(2,0,0), glm::vec3(0,1,0), 1);

}

void TaskAssignment_GravityPlanets::generateScene1()
{
    gEnv->stateSim->dtFixed = 0.001f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 10;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    ps.add(glm::vec3(0,0,0), glm::vec3(0,0,0), 100);
    ps.add(glm::vec3(2,0,0), glm::vec3(0,1,0), 3);
    ps.add(glm::vec3(2.1,0,0), glm::vec3(0,2.5,0), 0.3);
    ps.add(glm::vec3(3,0,0), glm::vec3(0,1,0), 2);
    ps.add(glm::vec3(3.1,0,0), glm::vec3(0,2.5,0), 0.3);
    ps.add(glm::vec3(4,0,0), glm::vec3(0,1,0), 1);
    ps.add(glm::vec3(4.1,0,0), glm::vec3(0,2.5,0), 0.3);

}

void TaskAssignment_GravityPlanets::generateScene2()
{
    gEnv->stateSim->dtFixed = 0.001f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 10;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    // Have fun here, if you like.
    // todo students
}


void TaskAssignment_GravityPlanets::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);

    ImGui::Separator();

    ImGui::SliderFloat(paramName("GravityIntensity"), &m_gravityIntensity, 0.01f, 10.0f);

    ImGui::Separator();
    if (ImGui::Button(paramName("Generate Scene0"))) { generateScene0(); }
    if (ImGui::Button(paramName("Generate Scene1"))) { generateScene1(); }
    if (ImGui::Button(paramName("Generate Scene2"))) { generateScene2(); }


}

const char* TaskAssignment_GravityPlanets::toString() const
{
    std::stringstream ss;
    ss << "Gravity: Handle particles as planets" << "\n";
    ss << "Following Newton's theory of gravitation" << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskAssignment_GravityPlanets::draw() const
{
 
}
