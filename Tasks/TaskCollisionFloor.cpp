#include "TaskCollisionFloor.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include "../Context/GlobalEnvironment.h"

void TaskCollisionFloor::setForces()
{
    m_floor.setPosition(glm::vec3(0,gEnv->stateGui->floorY,0));
    m_floor.handleCollisionByVelocity(particleSystem(0), m_kn_normalFriction, m_kt_tangentialFriction);
}

void TaskCollisionFloor::doWork() 
{
    m_floor.setPosition(glm::vec3(0, gEnv->stateGui->floorY, 0));
}

void TaskCollisionFloor::draw() const
{
    /*
    glColor4f(0, 1, 1, 0.5);
    m_floor.draw();
    glColor4f(1, 1, 1, 1);
    */
}

void TaskCollisionFloor::imGui()
{
    ImGui::SliderFloat(paramName("kn_normalFriction"), &m_kn_normalFriction, 0.0f, 1.0f);
    ImGui::SliderFloat(paramName("kt_tangentialFriction"), &m_kt_tangentialFriction, 0.0f, 1.0f);
}

const char* TaskCollisionFloor::toString() const
{
    std::stringstream ss;
    ss << "Floor Collision for PS0\n";
    m_string = ss.str();
    return m_string.c_str();
}


