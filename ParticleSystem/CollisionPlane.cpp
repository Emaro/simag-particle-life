#include "CollisionPlane.h"

#include <imgui/imgui.h>

#include "ParticleSystem.h"
#include "../Helper/HelperDraw.h"

CollisionPlane::CollisionPlane()
	: m_p(glm::vec3(0))
	, m_n(glm::vec3(0, 1, 0))
{
}

CollisionPlane::~CollisionPlane()
{
}

void CollisionPlane::handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction, float kt_tangentialFriction)
{
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& states = ps.states();

    auto& planeP = m_p;
    auto& planeN = m_n;

    const glm::vec3 eps = 0.0001f * planeN;

    for (int i = 0; i < s; i++)
    {
        if (states[i].isStatic()) continue; // Do not collide static particles

        glm::vec3& p = pos[i];
        glm::vec3& v = vel[i];
        glm::vec3& f = forces[i];

        auto pq = p - planeP;
        auto h = glm::dot(planeN, pq);

        if (h >= 0) {
            continue;
        }

        p += planeN * -h + eps;
        v = glm::reflect(v, planeN);

        handleFriction(v, f, planeN, kn_normalFriction, kt_tangentialFriction);
    }
}

void CollisionPlane::handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction, float kt_tangentialFriction)
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

        auto pq = pos[i] - m_p;
        auto h = glm::dot(m_n, pq);

        if (h >= 0) continue;

        forces[i] -= m_n * h * forceStrength;
        handleFriction(vel[i], forces[i], m_n, kn_normalFriction, kt_tangentialFriction);
    }

}

void CollisionPlane::draw() const
{
    Helper::drawPlane(m_p, m_n);
}

void CollisionPlane::imGui(std::string const& pre)
{
    ImGui::Text("Plane Parameter");
    ImGui::SliderFloat3((pre + "Position").c_str(), &m_p[0], -5.0f, 5.0f);
    ImGui::SliderFloat3((pre + "Normal").c_str(), &m_n[0], -1.0f, 1.0f);
    if (glm::dot(m_n, m_n) == 0) { m_n = glm::vec3(0, 1, 0); }
    m_n = glm::normalize(m_n);

}

const char* CollisionPlane::toString() const
{
	return "Collision with Plane";
}


