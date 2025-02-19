#include "TaskLect02_MyFirstTask_Differentiation.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <math.h>

#include "../Context/GlobalEnvironment.h"
#include "../Helper/HelperDraw.h"


float TaskLect02_MyFirstTask_Differentiation::myFunc(float x) const
{
    switch (m_functionType)
    {
    default:
    case 0:
        return expf(-x * x);
    case 1:
        return sinf(x);
    case 2:
        return 0.2f * x * x;
    }
}

float TaskLect02_MyFirstTask_Differentiation::derivative(float x, float h) const
{
    // todo students
    float result = 0.0f;
    return result;
}


void TaskLect02_MyFirstTask_Differentiation::setForces()
{

}

void TaskLect02_MyFirstTask_Differentiation::doWork()
{

}

void TaskLect02_MyFirstTask_Differentiation::draw() const
{

    float xMin = m_xMin;
    float xMax = m_xMax;
    float dx = m_dx;
    float h = m_h;

    float x = xMin;

    glPointSize(gEnv->stateGui->psPointSize);

    // Plot function
    glColor4f(1.0f, 0.0f, 0.0f, 1);
    while (x < xMax)
    {
        float y = myFunc(x);
        glm::vec3 pos(x, y, 0);
        Helper::drawPoint(pos);
        x += dx;
    }

    // Plot derivative
    x = m_xMin;
    glColor4f(0.5f, 0.5f, 0.5f, 1);
    if (m_plotDerivative)
    {
        // todo students
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1);
    glPointSize(1.0f);
}

void TaskLect02_MyFirstTask_Differentiation::copyToParticleSystem0()
{
    // particleSystem(0).clear();

    float xMin = m_xMin;
    float xMax = m_xMax;
    float dx = m_dx;
    float h = m_h;

    float x = xMin;

    // Add function as position & derivative as velocity to particlesystem 0

    // todo students

}

void TaskLect02_MyFirstTask_Differentiation::imGui()
{
    ImGui::SliderInt(paramName("Function Type"), &m_functionType, 0, 2);

    ImGui::Separator();
    ImGui::SliderFloat(paramName("xMin"), &m_xMin, -10.0f, -1.0f);
    ImGui::SliderFloat(paramName("xMax"), &m_xMax, 1.0f, 10.0f);
    ImGui::SliderFloat(paramName("dx"), &m_dx, 0.01f, 1.0f);
    ImGui::SliderFloat(paramName("h"), &m_h, 0.001f, 1.00f);

    ImGui::Separator();
    ImGui::Checkbox(paramName("Plot Analytical Derivative"), &m_plotDerivative);

    ImGui::Separator();
    if (ImGui::Button(paramName("Copy to PS0"))) { copyToParticleSystem0(); }
}

const char* TaskLect02_MyFirstTask_Differentiation::toString() const
{
    std::stringstream ss;
    ss << "Plots function and derivate of exp(-x*x)." << "\n";
    ss << "Allows for copying values into PS0." << "\n";
    ss << "Thereby, velocities are set by the derivative." << "\n";

    m_string = ss.str();
    return m_string.c_str();
}


