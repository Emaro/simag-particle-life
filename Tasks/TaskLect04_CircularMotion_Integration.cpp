#include "TaskLect04_CircularMotion_Integration.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include "../Context/GlobalEnvironment.h"

#include "../Helper/HelperDraw.h"

void TaskLect04_CircularMotion_Integration::setForces()
{
    auto& ps = particleSystem(0);

    if (ps.size() != 1)
    {
        std::cout << name() << " - Insufficient Particle Size! Press 'Reset'!\n";
        return;
    }

    // Circle
    auto& p0 = ps.particle(0); // Get particle 0

    // Add force in p0.force()...

    // todo students

}

void TaskLect04_CircularMotion_Integration::doWork() 
{
    if (!gEnv->stateSim->isRunning) return;

    auto& ps = particleSystem(0);

    if (ps.size() != 1)
    {
        std::cerr << "Insufficient Particle Size! Press 'Reset'!\n";
        return;
    }
}

void TaskLect04_CircularMotion_Integration::imGui()
{
    if (ImGui::Button(paramName("Reset")))
    { 
        auto& ps = particleSystem(0);
        ps.clear();

        // Circle
        ps.add(glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1));
        if (ps.size() != 1) { return; }

        auto& p0 = ps.particle(0); // Get particle 0

        // todo students

    }
}

const char* TaskLect04_CircularMotion_Integration::toString() const
{
    std::stringstream ss;
    ss << "Circular Force Field" << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskLect04_CircularMotion_Integration::draw() const
{
    // Draw sphere as reference
    glColor4f(0, 0, 1, 1);
    int sphereDiscretization = 50;
    Helper::drawCircle2d(glm::vec3(0), 1.0f, sphereDiscretization);

    glColor4f(1, 1, 1, 1);
}
