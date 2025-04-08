#include "TaskMouseAddVelocity.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../Helper/HelperDraw.h"

glm::vec2 TaskMouseAddVelocity::circleCenterFromMouse() const
{
    glm::vec2 center = gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos);
    return center;
}

void TaskMouseAddVelocity::setForces()
{
    auto& ps = particleSystem(m_workOnPsIdx);

    for (int i = 0; i < (int)ps.size(); i++) // Run over all particles
    {
        Particle& p = ps.particle(i); // Get Particle i

        glm::vec3& pos = p.pos(); // Reference to position
        glm::vec3& vel = p.vel(); // Reference to velocity

        const glm::vec2 circleCenter = circleCenterFromMouse();
        const glm::vec3 circleCenter3d(circleCenter, 0.0f);
        const float radius = m_radius;

        // todo students
        if (glm::length(pos - circleCenter3d) < radius)
            vel = vel
                + m_intensity
                    * 0.5f *(glm::cos(glm::pi<float>() * glm::length(pos - circleCenter3d) / radius) + 1)
                    * glm::normalize(pos - circleCenter3d);

        vel *= m_damping;
    }
}

void TaskMouseAddVelocity::doWork() 
{

}

void TaskMouseAddVelocity::draw() const
{
    glm::vec2 center = circleCenterFromMouse();
    Helper::drawCircle2d(glm::vec3(center,0), m_radius, 50);
}

void TaskMouseAddVelocity::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();
    ImGui::SliderFloat(paramName("Radius"), &m_radius, 0.1f, 3.0f);
    ImGui::SliderFloat(paramName("Intensity"), &m_intensity, 0.01f, 1.0f);
    ImGui::Separator();
    ImGui::SliderFloat(paramName("Velocity Damping"), &m_damping, 0.8f, 1.0f);
}

const char* TaskMouseAddVelocity::toString() const
{
    std::stringstream ss;
    ss << "Add velocity to particles in circle around mouse-pos.\n";
    ss << "Additionally, adds simple damping to all velocities of all particles.\n";
    ss << "- Therefore, velocities are reduced slightly per step (by factor < 1.0).\n";
    ss << "--------------------\n";
    ss << "Works on ParticleSystem 0 as default." << "\n";

    m_string = ss.str();
    return m_string.c_str();
}


