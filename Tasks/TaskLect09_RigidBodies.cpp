#include "TaskLect09_RigidBodies.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../Helper/HelperDraw.h"

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////

class RigidBody
{
public:
    RigidBody(ParticleSystem& ps, glm::vec3 const& initialAngularVelocity)
        : m_ps(ps)
        , m_particleInteractionForce(0)
        , m_center(0)
        , m_velocity(0)
        , m_angularVelocity(initialAngularVelocity)
        , m_I(1.0f)
        , m_Iinv(1.0f)
        , m_orientation(1.0f)
        , m_force(0)
        , m_torque(0)
    {
        m_sizeInit = ps.size();
        init();
    }

    void clearForcesAndAddGravity(float gravity);
    void integrate();
    void updateRBFromPS();
    void updatePSFromRB();

    void floorCollision(float forceScale, float forceDamping, float forceFriction);

    // used for interaction
    void setExternalInteractionForce(int pIdx, glm::vec3 force = glm::vec3(0))
    {
        m_particleInteractionIdx = pIdx;
        m_particleInteractionForce = force;
    }

    void draw(bool bDrawAxis, bool bDrawVelocity, bool bDrawPositionsInit) const;
    size_t size() const { return m_sizeInit; }

private:
    ParticleSystem& m_ps;

    // Interaction
    int m_particleInteractionIdx = -1;
    glm::vec3 m_particleInteractionForce{};

    // Rigid Body
    size_t m_sizeInit = 0;
    glm::vec3 m_center;
    glm::vec3 m_velocity;
    glm::vec3 m_angularVelocity;
    float m_mass = 1.0f;
    glm::mat3 m_I; // inertia tensor
    glm::mat3 m_Iinv; // inverted inertia tensor
    glm::mat3 m_orientation;
    glm::vec3 m_force;
    glm::vec3 m_torque;

    std::vector<glm::vec3> m_positionInit; // relative positions
    void determineCenterAndMassByParticles(); // center and mass of RB
    void init();
    void drawAxis(glm::vec3 const& pos, glm::mat3 const& axis) const; // todo -> Helper
};

void RigidBody::determineCenterAndMassByParticles()
{
    glm::vec3 center(0);
    float mass = 0.0f;
    const int n = (int)m_ps.size();
    if (n == 0) return;

    auto& pPositions = m_ps.positions();
    auto& pStates = m_ps.states();
    for (int i = 0; i < n; i++)
    {
        glm::vec3 const& p = pPositions[i];
        float pMass = pStates[i].mass();
        center += pMass * p;
        mass += pMass;
    }

    center = center / mass;

    m_center = center;
    m_mass = mass;
}

void RigidBody::init()
{
    // determine relative positions
    // determine center
    m_positionInit.clear();
    const int n = (int)m_ps.size();
    if (n == 0) return;

    determineCenterAndMassByParticles();

    // determine relative positions of particles according to center of RB
    for (int i = 0; i < n; i++)
    {
        glm::vec3 const& p = m_ps.positions()[i];
        m_positionInit.push_back(p - m_center);
    }

    // compute intertia tensor
    m_I = glm::mat3(1.0f);
    for (int i = 0; i < (int)m_ps.size(); i++)
    {
        glm::vec3 const& p = m_ps.positions()[i];
        float pMass = m_ps.states()[i].mass();

        glm::vec3 r = p - m_center;
        // skew matrix of r
        glm::mat3 skew_r(glm::vec3(0, r.z, -r.y), glm::vec3(-r.z, 0, r.x), glm::vec3(r.y, -r.x, 0));
        m_I = m_I - (skew_r * skew_r) * pMass;
    }

    // inverted inertia tensor
    m_Iinv = glm::inverse(m_I);

    // obtain an arbitrary orthonormal orientation matrix
    m_orientation = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
}

void RigidBody::draw(bool bDrawAxis, bool bDrawVelocity, bool bDrawPositionsInit) const
{
    if (m_positionInit.empty()) return;

    if (bDrawAxis) { drawAxis(m_center, m_orientation); }

    // draw velocity of RB
    if (bDrawVelocity)
    {
        glBegin(GL_LINES);
        glColor4f(0, 1, 1, 1);
        glVertex3fv(glm::value_ptr(m_center));
        glVertex3fv(glm::value_ptr(m_center + m_velocity));
        glEnd();
    }

    // draw lines to intertial positions
    if (bDrawPositionsInit)
    {
        glColor4f(0, 0, 1, 1);
        glBegin(GL_LINES);
        glm::vec3 temp;
        for (auto const& it : m_positionInit)
        {
            temp = m_center + it;
            glVertex3fv(glm::value_ptr(m_center));
            glVertex3fv(glm::value_ptr(temp));
        }
        glEnd();
    }

    glColor4f(1, 1, 1, 1);
}

void RigidBody::drawAxis(glm::vec3 const& pos, glm::mat3 const& axis) const
{
    glLineWidth(3.0f);
    glColor4f(0, 0, 1, 1); 
    glBegin(GL_LINES);
    glm::vec3 temp;
    for (int i = 0; i < 3; i++)
    {
        temp = pos + axis[i];
        switch (i)
        {
        default:
        case 0: glColor4f(1, 0, 0, 1); break;
        case 1: glColor4f(0, 1, 0, 1); break;
        case 2: glColor4f(0, 0, 1, 1); break;
        }
        glVertex3fv(glm::value_ptr(pos));
        glVertex3fv(glm::value_ptr(temp));
    }
    glEnd();
    glColor4f(1, 1, 1, 1);
    glLineWidth(1.0f);
}

void RigidBody::clearForcesAndAddGravity(float gravity)
{
    m_force = glm::vec3(0);
    m_torque = glm::vec3(0);
    m_force = glm::vec3(0, gravity * m_mass, 0);
}

void RigidBody::integrate()
{
    const float& dt = gEnv->stateSim->dtFixed;

    // sum the forces and torques of all particles to obtain the total force and torque
    auto& pos = m_ps.positions();
    auto& vel = m_ps.velocities();
    auto& forces = m_ps.forces();

    // sum up all forces
    for (int i = 0; i < (int)m_ps.size(); i++)
    {
        m_force += forces[i];
        m_torque += glm ::cross((pos[i] - m_center), forces[i]);
    }

    // interaction - add force into direction of mouse click
    if (m_particleInteractionIdx >= 0 && m_particleInteractionIdx < (int)m_ps.size())
    {
        m_force += m_particleInteractionForce * m_mass;
        m_torque += glm::cross((pos[m_particleInteractionIdx] - m_center), m_particleInteractionForce * m_mass);
    }

    // integrate (euler-cromer)
    m_velocity += m_force / m_mass * dt;
    m_center += m_velocity * dt;

    // compute the transposed orientation matrix
    glm::mat3 AT = glm::transpose(m_orientation);

    // compute the term A * I-1 * AT tau
    glm::vec3 ATtau = AT * m_torque;
    glm::vec3 JATtau = m_Iinv * ATtau;
    glm::vec3 AJATtau = m_orientation * JATtau;

    // integrate the angular velocity
    m_angularVelocity += AJATtau * dt;

    // skew matrix of the angular velocity / slide 8 "angular velocity" [this here is the transposed matrix]
    glm::mat3 skew_omega(
        glm::vec3(0, m_angularVelocity.z, -m_angularVelocity.y),
        glm::vec3(-m_angularVelocity.z, 0, m_angularVelocity.x),
        glm::vec3(m_angularVelocity.y, -m_angularVelocity.x, 0)
    );

    // numerically integrate the orientation
    m_orientation = m_orientation + skew_omega * m_orientation * dt;

    // re-orthonormalize the orientation matrix - Gram-Schmidt orthonormalization
    glm::vec3 a1 = m_orientation[0];
    glm::vec3 a2 = m_orientation[1];
    //glm::vec3 a3 = m_orientation[2];

    glm::vec3 b1 = glm::normalize(a1);
    glm::vec3 b2 = glm::normalize(a2 - b1 * glm::dot(b1, a2));
    glm::vec3 b3 = glm::normalize(glm::cross(b1, b2));
    m_orientation = glm::mat3(b1, b2, b3);

}

void RigidBody::updateRBFromPS()
{
    if (m_ps.size() <= 1) return;

    determineCenterAndMassByParticles();

    // update velocity
    m_velocity = glm::vec3(0);
    for (int i = 0; i < (int)m_positionInit.size(); i++)
    {
        m_velocity += m_ps.velocities()[i];
    }
    m_velocity /= (float)m_ps.size();

}

void RigidBody::updatePSFromRB()
{
    // update the positions of each particle based on the orientation of the rigid body
    for (int i = 0; i < (int)m_positionInit.size(); i++)
    {
        m_ps.positions()[i] = m_center + m_orientation * m_positionInit[i];
    }
}

void RigidBody::floorCollision(float forceScale, float forceDamping, float forceFriction)
{
    const glm::vec3 up(0, 1, 0); // floor up vector

    // add penalty forces
    const float deltaFloorForRenderingReflection = 0.1f;
    const float floorY = gEnv->stateGui->floorY + deltaFloorForRenderingReflection;

    for (int i=0; i<(int)m_ps.size(); i++)
    {
        auto& pPos = m_ps.positions()[i];
        float dist = pPos.y - floorY;
        if (dist<0)
        {
            auto& pForce = m_ps.forces()[i];
            pForce.y = -dist * forceScale;

            glm::vec3 pVel = m_velocity + glm::cross(m_angularVelocity, pPos-m_center);
            glm::vec3 pVelNormal = glm::dot(pVel, up) * up;

            pForce -= pVelNormal * forceDamping;

            glm::vec3 pVelTangential = pVel - pVelNormal;
            pForce -= pVelTangential * forceFriction;
        }
    }
}

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////

void TaskLect09_RigidBodies::generateRigidBody()
{
    rb = std::make_shared<RigidBody>(particleSystem(m_workOnPsIdx), m_initialAngularVelocity);
}

void TaskLect09_RigidBodies::setForces()
{
    auto& ps = particleSystem(m_workOnPsIdx);

    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    const glm::vec3 zero(0);

    if (!rb) return;
    if (rb->size() != ps.size()) return;

    // clear particle forces ==> i.e., disable ALL other tasks & avoid external integration!
    for (int i = 0; i < (int)ps.size(); i++)
    {
        vel[i] = zero;
        forces[i] = zero;
    }

    rb->clearForcesAndAddGravity(m_gravity);

    rb->floorCollision(m_floorCollisionForceScale, m_floorCollisionForceDamping, m_floorCollisionForceFriction); // determine forces for collisions

    rb->integrate();

    rb->updatePSFromRB();

    // set all forces&velocities to zero => avoid external integration
    for (int i = 0; i < (int)ps.size(); i++)
    {
        vel[i] = zero;
        forces[i] = zero;
    }
}

void TaskLect09_RigidBodies::doWork() 
{
    auto& ps = particleSystem(m_workOnPsIdx);

    if (!rb || rb->size() != ps.size())
    {
        generateRigidBody();
    }

    // interaction
    if (!gEnv->stateGui->mouseButtonLeftPressed)
    {
        m_closestParticleIdx = -1;
        rb->setExternalInteractionForce(-1);
    }
    if (gEnv->stateGui->mouseButtonLeftPressed)
    {
        if (m_closestParticleIdx == -1)
        {
            findClosestParticle();
            if (m_closestParticleIdx>0) m_closestParticlePosOnClick = ps.positions()[m_closestParticleIdx];
        }
        if (m_closestParticleIdx >= 0)
        {
            const glm::vec3 deltaDist = gEnv->camera->SSToWorld3d(gEnv->stateGui->mousePos, m_closestParticlePosOnClick) - ps.positions()[m_closestParticleIdx];
            rb->setExternalInteractionForce(m_closestParticleIdx, deltaDist * m_interactionForceIntensity);
        }
    }
}

void TaskLect09_RigidBodies::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();
    ImGui::SliderFloat3(paramName("m_initialAngularVelocity"), glm::value_ptr(m_initialAngularVelocity), 0.0f, 5.0f);
    if (ImGui::Button(paramName("Generate Rigid Body"))) { generateRigidBody(); }
    
    ImGui::Separator();
    ImGui::SliderFloat(paramName("m_interactionForceIntensity"), &m_interactionForceIntensity, 1.0f, 40.0f);

    ImGui::Separator();
    ImGui::SliderFloat(paramName("m_gravity"), &m_gravity, -9.81f, -1.0f);

    // floor collision
    ImGui::Separator();
    ImGui::SliderFloat(paramName("m_floorCollisionForceScale"), &m_floorCollisionForceScale, 1.0f, 5000.0f);
    ImGui::SliderFloat(paramName("m_floorCollisionForceDamping"), &m_floorCollisionForceDamping, 1.0f, 200.0f);
    ImGui::SliderFloat(paramName("m_floorCollisionForceFriction"), &m_floorCollisionForceFriction, 1.0f, 200.0f);

    ImGui::Separator();
    ImGui::Checkbox(paramName("m_rbDrawAxis"), &m_rbDrawAxis);
    ImGui::Checkbox(paramName("m_rbDrawVelocity"), &m_rbDrawVelocity);
    ImGui::Checkbox(paramName("m_rbDrawPositionsInit"), &m_rbDrawPositionsInit);
}

const char* TaskLect09_RigidBodies::toString() const
{
    std::stringstream ss;
    ss << "Rigid Body" << "\n";
    ss << "This Task is stand-alone" << "\n";
    ss << "-> No other tasks are needed" << "\n";
    ss << "-> (nor should be active!)" << "\n";
    ss << "Ideally: Set Integrator to empty." << "\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskLect09_RigidBodies::draw() const
{
    if (rb) { rb->draw(m_rbDrawAxis, m_rbDrawVelocity, m_rbDrawPositionsInit); }
}

// Basically copied from TaskPickParticle.cpp! Used for interaction.
void TaskLect09_RigidBodies::findClosestParticle()
{
    // Search for closest particle according to mouse position, returns idx

    auto* cam = gEnv->camera;
    const int width = cam->sizeWindow().x;
    const int height = cam->sizeWindow().y;
    glm::vec3 posMouse(gEnv->stateGui->mousePos, 0.0f);
    posMouse.x = posMouse.x / (float)width * 2.0f - 1.0f;
    posMouse.y = ((float)height - posMouse.y) / (float)height * 2.0f - 1.0f;

    glm::mat4 m = cam->getProjection() * cam->getView();

    ParticleSystem& ps = particleSystem(m_workOnPsIdx);

    float minDistSqr = FLT_MAX;
    int idx = 0;
    for (auto const& it : ps.positions())
    {
        glm::vec4 temp4 = m * glm::vec4(it, 1.0f);
        glm::vec3 temp = glm::vec3(temp4) / temp4.w;
        temp.z = 0;
        temp = temp - posMouse;
        float distSqr = glm::dot(temp, temp);
        if (distSqr < minDistSqr)
        {
            m_closestParticleIdx = idx;
            minDistSqr = distSqr;
        }
        idx++;
    }
}