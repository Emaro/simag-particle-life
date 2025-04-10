#include "CollisionCube.h"

#include <imgui/imgui.h>

#include "ParticleSystem.h"
#include "../Helper/HelperDraw.h"

namespace
{
    void ensureMinMax(glm::vec3& p0, glm::vec3& p1)
    {
        glm::vec3 a = p0;
        glm::vec3 b = p1;
        for (int i = 0; i < 3; i++)
        {
            p0[i] = std::min<float>(a[i], b[i]);
            p1[i] = std::max<float>(a[i], b[i]);
        }
    }

    void getSmallestAxis(glm::vec3& c0, glm::vec3& c1, glm::vec3& p, glm::vec3& normal, float& dist)
    {
        glm::vec3 p0 = p - c0;
        glm::vec3 p1 = c1 - p;

        glm::vec3 pq;

        if (p0.x < p0.y && p0.x < p0.z && p0.x < p1.x && p0.x < p1.y && p0.x < p1.z)
        {
            normal = glm::vec3(-1, 0, 0);
            pq = p - c0;
        }
        else if (p0.y < p0.z && p0.y < p1.x && p0.y < p1.y && p0.y < p1.z)
        {
            normal = glm::vec3(0, -1, 0);
            pq = p - c0;
        }
        else if (p0.z < p1.x && p0.z < p1.y && p0.z < p1.z)
        {
            normal = glm::vec3(0, 0, -1);
            pq = p - c0;
        }
        else if (p1.x < p1.y && p1.x < p1.z)
        {
            normal = glm::vec3(1, 0, 0);
            pq = p - c1;
        }
        else if (p1.y < p1.z)
        {
            normal = glm::vec3(0, 1, 0);
            pq = p - c1;
        }
        else {
            normal = glm::vec3(0, 0, 1);
            pq = p - c1;
        }

        dist = -glm::dot(normal, pq);
    }
}

CollisionCube::CollisionCube()
    : m_p0(glm::vec3(0, 0, -1))
    , m_p1(glm::vec3(3, 1, 1))
{
    ensureMinMax(m_p0, m_p1);
}

CollisionCube::~CollisionCube()
{
}

void CollisionCube::handleCollisionByVelocity(ParticleSystem& ps, float kn_normalFriction, float kt_tangentialFriction)
{
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& states = ps.states();
    
    auto& cubeP0 = m_p0;
    auto& cubeP1 = m_p1;

    const float eps = 0.0001f;
    
    glm::vec3 cubeN(0);
    float cubeDistToSurface;

    for (int i = 0; i < s; i++)
    {
        if (states[i].isStatic()) continue; // Do not collide static particles

        glm::vec3& p = pos[i];
        glm::vec3& v = vel[i];
        glm::vec3& f = forces[i];

        // For a particle to be in the cube, it's coordinates need to be
        // between p0 and p1 on all axis.

        bool isInCube = cubeP0.x < p.x && cubeP0.y < p.y && cubeP0.z < p.y
                     && cubeP1.x > p.x && cubeP1.y > p.y && cubeP1.z > p.y;

        if (!isInCube) continue;

        // Now we want to find the side of the cube the particle is closest to

        getSmallestAxis(cubeP0, cubeP1, p, cubeN, cubeDistToSurface);

        if (cubeDistToSurface <= 0) continue;

        p += cubeN * (cubeDistToSurface + eps);
        v = glm::reflect(v, cubeN);

        handleFriction(v, f, cubeN, kn_normalFriction, kt_tangentialFriction);
    }
}

void CollisionCube::handleCollisionByForce(ParticleSystem& ps, float forceStrength, float kn_normalFriction, float kt_tangentialFriction)
{
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& states = ps.states();

    auto& cubeP0 = m_p0;
    auto& cubeP1 = m_p1;

    const float eps = 0.0001f;

    glm::vec3 cubeN(0);
    float cubeDistToSurface;

    for (int i = 0; i < s; i++)
    {
        if (states[i].isStatic()) continue; // Do not collide static particles

        glm::vec3& p = pos[i];
        glm::vec3& v = vel[i];
        glm::vec3& f = forces[i];

        bool isInCube = cubeP0.x < p.x && cubeP0.y < p.y && cubeP0.z < p.y
                     && cubeP1.x > p.x && cubeP1.y > p.y && cubeP1.z > p.y;

        if (!isInCube) continue;

        getSmallestAxis(cubeP0, cubeP1, p, cubeN, cubeDistToSurface);

        if (cubeDistToSurface <= 0) continue;

        f += cubeN * (cubeDistToSurface + eps);

        handleFriction(v, f, cubeN, kn_normalFriction, kt_tangentialFriction);
    }
}

void CollisionCube::draw() const
{
    Helper::drawCube(m_p0, m_p1);
}

void CollisionCube::imGui(std::string const& pre)
{
    ImGui::Text("Cube Parameter");
    ImGui::SliderFloat3((pre + "P0").c_str(), &m_p0[0], -5.0f, 5.0f);
    ImGui::SliderFloat3((pre + "P1").c_str(), &m_p1[0], -5.0f, 5.0f);

    glm::vec3 center = (m_p0 + m_p1) * 0.5f;
    ImGui::SliderFloat3((pre + "Center").c_str(), &center[0], -5.0f, 5.0f);
    glm::vec3 dx = (m_p1 - m_p0)*0.5f; m_p0 = center-dx; m_p1 = center + dx;

    ensureMinMax(m_p0, m_p1);
}

const char* CollisionCube::toString() const
{
    return "Collision with Cube";
}


