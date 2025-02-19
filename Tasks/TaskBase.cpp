#include "TaskBase.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../ParticleSystem/ParticleSystem.h"
#include "../Helper/HelperDraw.h"

// Add more headers here if you want...

namespace
{

    // Add your local functions here...

}

void TaskBase::setForces()
{
    // Set forces & velocities of particles here...
    // E.g.:

    /*
    auto& ps0 = particleSystem(0); // Get particle system 0 -> ps0
    size_t s = ps0.size();         // Number of particles in ps0
    if (ps0.size() == 0) return;   // Nothing to do

    // Work on particles, e.g. set all velocities to zero (0,0,0)
    for (int i = 0; i < s; i++)
    {
        ps0.particle(i).vel() = glm::vec3(0.0f, 0.0f, 0.0f);
        // ps0.particle(i).force() = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    */
}

void TaskBase::doWork() 
{
    // Anything, which is not force/velocity-related (i.e., timestep related) comes here.
    // Access of particles similiar to "setForces()".
    // This method is called every frame, not only when simulation is running.

    // E.g., change colors of all particles:
    
    if (!m_boolTest) return;       // Early exit -> do nothing, if checkbox is not set

    auto& ps0 = particleSystem(0); // Get particle system 0 -> ps0
    size_t s = ps0.size();         // Number of particles in ps0
    if (ps0.size() == 0) return;   // Nothing to do


    // Work on particles, e.g. animate colors
    m_count += 0.04f;
    glm::vec4 newColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (sinf(m_count) * 0.5f + 0.5f); // Set color according to sine (colors range from 0.0 .. 1.0)
    newColor.a = 1.0f; // ensure alpha = 1.0
    for (int i = 0; i < (int)s; i++)
    {
        newColor.r = sinf(ps0.particle(i).pos().x) * 0.3f + 0.7f; // Set red channel according to x pos
        ps0.particle(i).color() = newColor;
    }
}

void TaskBase::draw() const
{
    if (m_boolTest)
    {
        // Draw diagonal line in red
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth(5.0f);
        Helper::drawLine2d(-5.0f, -5.0f, 5.0f, 5.0f);
        glLineWidth(1.0f);
    }
}

void TaskBase::imGui()
{
    ImGui::Text("%s", paramName("Test Text"));
    if (ImGui::Button(paramName("TestButton")))
    {
        std::cerr << "Test Button pressed\n"; // print to terminal

        // Add particle

        // todo students
    }
    ImGui::Checkbox(paramName("TestCheckbox: Draw red line & Animate Colors"), &m_boolTest);
    ImGui::SliderFloat(paramName("TestSliderFloat"), &m_sliderValue, 0.0f, 1.0f);
}

const char* TaskBase::toString() const
{
    std::stringstream ss;

    ss << "Description:" << "\n";
    ss << "A base for your own tasks..." << "\n";
    ss << "" << "\n";

    if (m_boolTest)
    {
        ss << "Checkbox: IS CHECKED => draw red line\n";
    }
    else
    {
        ss << "Checkbox: NOT CHECKED\n";
    }

    ss << "SliderFloat Value: " << m_sliderValue << "\n";

    m_string = ss.str();
    return m_string.c_str();
}


