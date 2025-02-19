#include "TaskLect03_CarCollision.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Context/GlobalEnvironment.h"


void TaskLect03_CarCollision::setForces()
{
    if (!gEnv->stateSim->isRunning) return;

    if (particleSystem(0).size() != 2) return;

    // todo students
}

void TaskLect03_CarCollision::doWork()
{

}

void TaskLect03_CarCollision::draw() const
{
}

void TaskLect03_CarCollision::imGui()
{
    if (ImGui::Button(paramName("Do Setup")))
    {
        m_time = 0;

        particleSystem(0).clear();

        // todo students
    }

    ImGui::Separator();

    ImGui::SliderFloat(paramName("Car0 x"), &m_x0, -100.0f, -1.0f);
    ImGui::SliderFloat(paramName("Car0 vel"), &m_vel0, 1.0f, 30.0f);
    ImGui::ColorEdit4(paramName("Car0 color"), glm::value_ptr(m_col0));
    ImGui::Separator();
    ImGui::SliderFloat(paramName("Car1 x"), &m_x1, 1.0f, 100.0f);
    ImGui::SliderFloat(paramName("Car1 vel"), &m_vel1, -1.0f, -30.0f);
    ImGui::ColorEdit4(paramName("Car1 color"), glm::value_ptr(m_col1));
}

const char* TaskLect03_CarCollision::toString() const
{
    std::stringstream ss;
    ss << "Description:" << "\n";
    ss << "Car-collision-example from lecture02..." << "\n";
    ss << "To get the exact results like in the cpp.sh example: Set dtFixed to 0.01." << "\n";
    ss << "" << "\n";
    ss << "dt: " << gEnv->stateSim->dt << "\n";
    ss << "" << "\n";

    if (particleSystem(0).size() == 2)
    {
        float x0 = particleSystem(0).particle(0).pos().x;
        float x1 = particleSystem(0).particle(1).pos().x;

        ss << "Car0 x:   " << x0 << "\n";
        ss << "Car0 vel: " << m_vel0 << "\n";
        ss << "Car1 x:   " << x1 << "\n";
        ss << "Car1 vel: " << m_vel1 << "\n";
        ss << "--------------------\n";
        ss << "Time: " << m_time << "\n";
        ss << "Avg pos car0&car1: " << (x0 + x1) / 2.0f << "\n";
    }


    m_string = ss.str();
    return m_string.c_str();
}


