#include "TaskLect05_Springs.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include "../Context/GlobalEnvironment.h"

#include "../Helper/HelperDraw.h"

namespace
{
    int getIdx(int x, int y, int nX, int nY) 
     { 
         if (x < 0) return -1; 
         if (y < 0) return -1; 
         if (x >= nX) return -1; 
         if (y >= nY) return -1; 
         return x + y * nX; 
     } 
}



bool TaskLect05_Springs::isValidParticleIdx(int idx) const
{
    if (idx >= 0 && idx < (int)particleSystem(m_workOnPsIdx).size())
    {
        return true;
    }
    return false;
}

void TaskLect05_Springs::addNeighbor(int idx0, int idx1)
{
    int idxMin = std::min<int>(idx0, idx1);
    int idxMax = std::max<int>(idx0, idx1);
    if (!isValidParticleIdx(idxMin)) return;
    if (!isValidParticleIdx(idxMax)) return;
    auto& ps = particleSystem(m_workOnPsIdx);
    ps.states()[idxMin].neighborList().push(Neighbor(idxMax, glm::length(ps.positions()[idxMin]- ps.positions()[idxMax]))); // only add neighbor from small idx -> large idx
}

void TaskLect05_Springs::addParticle(glm::vec3 const& pos, bool isStatic)
{
    const glm::vec4 colDefault(1, 1, 1, 1);
    const glm::vec4 colStatic(1, 0, 0, 1);
    auto& ps = particleSystem(m_workOnPsIdx);
    ps.add(pos, isStatic ? colStatic : colDefault);
    ps.particleLast().setStatic(isStatic);
}


void TaskLect05_Springs::generateScene0_pendula()
{
    // Pendula
    m_kSpring = 100.0f;
    m_dampingSpring = 5.0f;
    gEnv->stateSim->dtFixed = 0.006f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 30;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    addParticle(glm::vec3(0, 1, 0), true); // attach first particle
    addParticle(glm::vec3(1, 1, 0), false);

    addNeighbor(0, 1);
}

void TaskLect05_Springs::generateScene1_rope()
{
    // Rope
    m_kSpring = 20.0f;
    m_dampingSpring = 5.0f;
    gEnv->stateSim->dtFixed = 0.006f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 30;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    const int nMax = m_ropeElementsHalf;

    // Add partcicles
    // Set first and last particle as static
    for (int i = -nMax; i <= nMax; i++)
    {
        addParticle(glm::vec3(i, 1, 0), i == -nMax || i == nMax && m_createBothEndsStatic);
    }

    // Add neighbors
    for (int i = -nMax; i < nMax; i++)
    {
        addNeighbor(nMax + i, nMax + i + 1);
    }

}

void TaskLect05_Springs::addBox(glm::vec3 const& p0, glm::vec3 const& p1)
{
    auto& ps = particleSystem(m_workOnPsIdx);
    int nextIdx = ps.size();

    for (int i = 0; i < 8; i++)
    {
        glm::vec3 p = glm::vec3(
            i & 1 ? p1.x : p0.x,
            i & 2 ? p1.y : p0.y,
            i & 4 ? p1.z : p0.z
        );

        ps.add(p);
    }

    for (int i = 0; i < 8; i++)
    for (int j = i+1; j < 8; j++)
    {
        addNeighbor(nextIdx + i, nextIdx + j);
    }
}

void TaskLect05_Springs::generateScene2_ragdoll()
{

    m_kSpring = 2000.0f;
    m_dampingSpring = 50.0f;
    gEnv->stateSim->dtFixed = 0.002f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 10;

    auto& ps = particleSystem(m_workOnPsIdx);
    auto currI = ps.size();
    //
    //  todo students
    // 0-7
    addBox(glm::vec3(-0.3, 0, -0.1), glm::vec3(-0.1, 0.4, .1));
    // 8-15
    addBox(glm::vec3(0.1, 0, -0.1), glm::vec3(.3, 0.4, .1));
    // 16-23
    addBox(glm::vec3(-0.3, 0.5, -0.1), glm::vec3(-0.1, 0.9, .1));
    // 24-31
    addBox(glm::vec3(0.1, 0.5, -0.1), glm::vec3(.3, 0.9, .1));


    addNeighbor(currI + 2, currI + 16 + 0);
    addNeighbor(currI + 3, currI + 16 + 1);
    addNeighbor(currI + 6, currI + 16 + 4);
    addNeighbor(currI + 7, currI + 16 + 5);
    addNeighbor(currI + 2, currI + 16 + 1);
    addNeighbor(currI + 3, currI + 16 + 0);
    addNeighbor(currI + 6, currI + 16 + 5);
    addNeighbor(currI + 7, currI + 16 + 4);

    addNeighbor(currI+8 + 2, currI+8 + 16 + 0);
    addNeighbor(currI+8 + 3, currI+8 + 16 + 1);
    addNeighbor(currI+8 + 6, currI+8 + 16 + 4);
    addNeighbor(currI+8 + 7, currI+8 + 16 + 5);
    addNeighbor(currI+8 + 2, currI+8 + 16 + 1);
    addNeighbor(currI+8 + 3, currI+8 + 16 + 0);
    addNeighbor(currI+8 + 6, currI+8 + 16 + 5);
    addNeighbor(currI+8 + 7, currI+8 + 16 + 4);

    addBox(glm::vec3(-0.3, 1.0, -0.2), glm::vec3(0.3, 1.8, .2));

    addNeighbor(currI+16 + 2, currI + 32 + 0);
    addNeighbor(currI+16 + 3, currI + 32 + 1);
    addNeighbor(currI+16 + 6, currI + 32 + 4);
    addNeighbor(currI+16 + 7, currI + 32 + 5);

    addNeighbor(currI+24 + 2, currI + 32 + 0);
    addNeighbor(currI+24 + 3, currI + 32 + 1);
    addNeighbor(currI+24 + 6, currI + 32 + 4);
    addNeighbor(currI+24 + 7, currI + 32 + 5);

    // 40-47
    addBox(glm::vec3(-1.2, 1.6, -.1), glm::vec3(-.35, 1.8, .1));
    // 48-55
    addBox(glm::vec3(.35, 1.6, -0.1), glm::vec3(1.2, 1.8, .1));

    addNeighbor(currI+40 + 1, currI + 32 + 0);
    addNeighbor(currI+40 + 3, currI + 32 + 2);
    addNeighbor(currI+40 + 5, currI + 32 + 4);
    addNeighbor(currI+40 + 7, currI + 32 + 6);

    addNeighbor(currI+48 + 0, currI + 32 + 1);
    addNeighbor(currI+48 + 2, currI + 32 + 3);
    addNeighbor(currI+48 + 4, currI + 32 + 5);
    addNeighbor(currI+48 + 6, currI + 32 + 7);

    // 56-63
    addBox(glm::vec3(-0.15, 1.85, -0.15), glm::vec3(0.15, 2.1, .15));

    addNeighbor(currI+32 +2, currI + 56 + 2);
    addNeighbor(currI+32 +3, currI + 56 + 3);
    addNeighbor(currI+32 +6, currI + 56 + 6);
    addNeighbor(currI+32 +7, currI + 56 + 7);

    addNeighbor(currI+32 + 2, currI + 56 + 0);
    addNeighbor(currI+32 + 3, currI + 56 + 1);
    addNeighbor(currI+32 + 6, currI + 56 + 4);
    addNeighbor(currI+32 + 7, currI + 56 + 5);
}


void TaskLect05_Springs::generateScene3_cloth()
{
    // Cloth
    m_kSpring = 20.0f;
    m_dampingSpring = 5.0f;
    gEnv->stateSim->dtFixed = 0.006f;
    gEnv->stateSim->dtFixedNoOfStepsPerFrame = 30;

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    const int nX = 50;
    const int nY = 51;
    const float height = 5.0f;
    const float scale = 0.1f;

    bool bCreate2SymmetricSpringsPerSpring = true;

    // 1. Create grid of particles with following coordinates:
    //    (float)x* scale, height, (float)y* scale) with x=0..nX-1 and y=0..nY-1
    //    set all particles with y==0 static!

    // 2. add 3 types of springs
    //    - stretch
    //    - shear
    //    - bend
    // use helper method getIdx to receive neighbor ids within your grid - e.g.,  int idx = getIdx(x, y, nX, nY);
    // add neighbor is safe, so you can try to add springs to non valid particle ids (e.g., -1) - then a spring would not be created
    // ==> thus, you don't have to take care of array-borders 
    // 

    for (int y = 0; y < nY; y++) { 
        for (int x = 0; x < nX; x++) { 
            addParticle(glm::vec3(x * scale - (nX * scale) / 2.0, height, y * scale - (nY*scale) / 2.0), false); 
        } 
    } 
 
    for (int y = 0; y < nY; y++) { 
        for (int x = 0; x < nX; x++) { 
            int idx = getIdx(x, y, nX, nY); 
 
            // create spring to particles on the right 
            addNeighbor(idx, getIdx(x+1, y, nX, nY)); 
            addNeighbor(idx, getIdx(x+2, y, nX, nY)); 
 
            // create spring to particles below 
            addNeighbor(idx, getIdx(x, y+1, nX, nY)); 
            addNeighbor(idx, getIdx(x, y+2, nX, nY)); 
 
            // create spring to particle to the bottom right 
            // addNeighbor(idx, getIdx(x+1, y+1, nX, nY)); 
        } 
    } 
}

void TaskLect05_Springs::generateScene4()
{
    // Bunny
}

void TaskLect05_Springs::generateSpringsAll()
{

    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clearAllNeighbors();

    int s = (int)ps.size();
    if (s < 1) return;

    for (int y = 0; y < s - 1; y++)
    {
        glm::vec3 p0 = ps.positions()[y];
        for (int x = y + 1; x < s; x++)
        {
            glm::vec3 p1 = ps.positions()[x];
            p1 = p1 - p0;
            float dist = sqrtf(glm::dot(p1, p1));
            ps.states()[y].neighborList().push(Neighbor(x, dist)); // only add neighbor from small idx -> large idx
        }
    }
}

namespace
{

}

void TaskLect05_Springs::setForces()
{

    auto& ps = particleSystem(m_workOnPsIdx);
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();

    const float kSpring = m_kSpring;       // spring constant
    const float kdSpring = m_dampingSpring; // damping constant
    const float eps = 0.0001f;

    glm::vec3 distVec;
    glm::vec3 forceVec;
    glm::vec3 relVel;

    for (int i = 0; i < s; i++)
    {
        glm::vec3& p0 = pos[i];
        glm::vec3& v0 = vel[i];
        glm::vec3& force0 = forces[i];
        auto& neighborList = ps.states()[i].neighborList().get();

        for (auto& n : neighborList)
        {
            auto dx = glm::length(pos[n.idx] - p0) - n.distance;
            auto dn = glm::normalize(pos[n.idx] - p0);
            auto fs = kSpring * dx * dn;

            force0 += fs;
            forces[n.idx] -= fs;
            
            if (m_bUseDampingSpring)
            {
                auto vd0 = dn * glm::dot(v0, dn);
                auto vd1 = dn * glm::dot(vel[n.idx], dn);
                auto fd = (1.0f) * kdSpring * (vd1 - vd0);

                force0 += fd;
                forces[n.idx] -= fd;
            }
        }
    }

}

void TaskLect05_Springs::doWork() 
{

}

void TaskLect05_Springs::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();
    ImGui::SliderFloat(paramName("kSpring"), &m_kSpring, 0.1f, 10.0f);
    ImGui::Separator();
    ImGui::Checkbox(paramName("UseDampingSpring"), &m_bUseDampingSpring);
    ImGui::SliderFloat(paramName("dampingSpring"), &m_dampingSpring, 0.01f, 1.0f);
    ImGui::Separator();
    if (ImGui::Button(paramName("Generate Springs All"))) { generateSpringsAll(); }
    ImGui::Separator();
    if (ImGui::Button(paramName("Generate Scene0 - Pendula"))) { generateScene0_pendula(); }
    if (ImGui::Button(paramName("Generate Scene1 - Rope")))    { generateScene1_rope(); }
    ImGui::SliderInt(paramName("m_ropeElementsHalf"), &m_ropeElementsHalf, 1, 10);
    ImGui::SliderInt(paramName("m_noOfRopeWithDifferentConnections [0..4]"), &m_noOfRopeWithDifferentConnections, 0, 4);
    ImGui::Checkbox(paramName("m_createBothEndsStatic"), &m_createBothEndsStatic);
    
    if (ImGui::Button(paramName("Generate Scene2 - Ragdoll"))) { generateScene2_ragdoll(); }
    if (ImGui::Button(paramName("Generate Scene3 - Cloth")))   { generateScene3_cloth(); }
    //if (ImGui::Button(paramName("Generate Scene4"))) { generateScene4(); }
    ImGui::Checkbox(paramName("m_drawSolid"), &m_drawSolid);
}

const char* TaskLect05_Springs::toString() const
{
    std::stringstream ss;
    ss << "Simulate springs." << "\n";


    m_string = ss.str();
    return m_string.c_str();
}

void TaskLect05_Springs::draw() const
{
}
