#include "TaskCollision.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../ParticleSystem/ParticleSystem.h"
#include "../Helper/HelperDraw.h"
#include "../ParticleSystem/CollisionPlane.h"

namespace
{

    // Add your local functions here...

}

void TaskCollision::setForces()
{
    auto& ps = particleSystem(0);

    if (m_handleCollisionByForce)
    {
        m_collider->handleCollisionByForce(ps, m_forceStrength, m_kn_normalFriction, m_kt_tangentialFriction);
    }
    else
    {
        m_collider->handleCollisionByVelocity(ps, m_kn_normalFriction, m_kt_tangentialFriction);
    }
}

void TaskCollision::doWork() 
{
    if (gEnv->stateSim->dtActualStepPerFrame != 0) return;

    if (m_animateCollider)
    {
        glm::vec3 pos = m_collider->position();

        // Rotate position around y axis with speed 'm_animateColliderSpeed'

        // todo students

        m_collider->setPosition(pos);
        
    }

}

void TaskCollision::imGui()
{
    ImGui::SliderFloat(paramName("m_kn_normalFriction"), &m_kn_normalFriction, 0.0f, 1.0f);
    ImGui::SliderFloat(paramName("kt_tangentialFriction"), &m_kt_tangentialFriction, 0.0f, 1.0f);

    ImGui::Separator();
    
    ImGui::Checkbox(paramName("m_handleCollisionByForce"), &m_handleCollisionByForce);
    if (m_handleCollisionByForce)
    {
        ImGui::SliderFloat(paramName("m_forceStrength"), &m_forceStrength, 0.0f, 1000.0f);
    }

    ImGui::Separator();
        
    ImGui::SliderInt(paramName("Collider Type"), &m_colliderType, 0, 2);
    setCollider();
    m_collider->imGui(paramName(""));

    ImGui::Separator();

    if (m_colliderType > 0) // do not animate plane
    {
        ImGui::Checkbox(paramName("m_animateCollider"), &m_animateCollider);
        if (m_animateCollider)
        {
            ImGui::SliderFloat(paramName("m_animateColliderSpeed"), &m_animateColliderSpeed, 0.01f, 0.1f);
        }
    }
    
}

void TaskCollision::draw() const
{
    glColor4f(0, 1, 1, 0.5);
    m_collider->draw();
    glColor4f(1, 1, 1, 1);
}

const char* TaskCollision::toString() const
{
    std::stringstream ss;

    ss << "Collision" << "\n";
    ss << "m_handleCollisionByForce -> if false, handle by velocities" << "\n";

    m_string = ss.str();
    return m_string.c_str();
}


