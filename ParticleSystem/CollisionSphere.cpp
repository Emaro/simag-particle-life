#include "CollisionSphere.h"

#include <imgui/imgui.h>

#include "ParticleSystem.h"
#include "../Helper/HelperDraw.h"

CollisionSphere::CollisionSphere()
    : m_p(glm::vec3(0))
    , m_r(1.0f)
{
}

CollisionSphere::~CollisionSphere()
{
}

void CollisionSphere::handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction, float kt_tangentialFriction)
{
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& states = ps.states();

    auto& sphereP = m_p;
    auto& sphereR = m_r;

    const float eps = 0.0001f;

    glm::vec3 vecToCenter(0);
    glm::vec3 sphereNormal(0);

    for (int i = 0; i < s; i++)
    {
        if (states[i].isStatic()) continue; // Do not collide static particles

        glm::vec3& p = pos[i];
        glm::vec3& v = vel[i];
        glm::vec3& f = forces[i];

        // todo students
        vecToCenter = p - sphereP;
        sphereNormal = glm::normalize(vecToCenter);
        float h = glm::length(vecToCenter) ;
        if (h > sphereR) continue;

        p += sphereNormal * (sphereR - h + eps);
        v = glm::reflect(v, sphereNormal);

        handleFriction(v, f, sphereNormal, kn_normalFriction, kt_tangentialFriction);
    }

}

void CollisionSphere::handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction, float kt_tangentialFriction)
{
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& states = ps.states();

    for (int i = 0; i < s; i++)
    {
        if (states[i].isStatic()) continue; // Do not collide static particles

        auto cp = pos[i] - m_p;
        glm::vec3 n = glm::normalize(cp);
        float h =  glm::length(cp) ;

        if (h > m_r) continue;

        forces[i] += n * (m_r - h) * forceStrength;
        handleFriction(vel[i], forces[i], n, kn_normalFriction, kt_tangentialFriction);
    }
}

void CollisionSphere::draw() const
{
    Helper::drawSphere(m_p, m_r);
}

void CollisionSphere::imGui(std::string const& pre)
{
    ImGui::Text("Sphere Parameter");
    ImGui::SliderFloat3((pre + "Position").c_str(), &m_p[0], -5.0f, 5.0f);
    ImGui::SliderFloat((pre + "Radius").c_str(), &m_r, 0.1f, 2.0f);
}

const char* CollisionSphere::toString() const
{
    return "Collision with Sphere";
}


