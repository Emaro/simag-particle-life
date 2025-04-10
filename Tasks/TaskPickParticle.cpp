#include "TaskPickParticle.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../ParticleSystem/ParticleSystemContainer.h"
#include "../Context/GlobalEnvironment.h"

bool TaskPickParticle::isValidParticleIdx(int idx) const
{
    if (idx >= 0 && idx < (int)particleSystem(m_workOnPsIdx).size())
    {
        return true;
    }
    return false;
}

void TaskPickParticle::rotateAllStaticParticlesAroundCenter()
{
    auto& ps = particleSystem(m_workOnPsIdx);
    float dxRotation = (gEnv->stateGui->mousePos.x - m_clickPos.x)/(float)gEnv->camera->sizeWindow().x * 2.0f * glm::pi<float>();
    m_clickPos = gEnv->stateGui->mousePos;
    int s = (int)ps.size();

    // todo students
    glm::vec3 center = glm::vec3(0);
    int numOfStatic = 0;
    for (int i = 0; i < s; i++)
    {
        if (ps.states()[i].isStatic())
        {
            center += ps.positions()[i];
            numOfStatic++;
        }
    }

    center = center / (float) numOfStatic;

    for (int i = 0; i < s; i++)
    {
        if (ps.states()[i].isStatic())
        {
            ps.positions()[i] -= center;
            ps.positions()[i] = glm::rotate(ps.positions()[i], dxRotation, glm::vec3(0,1,0));
            ps.positions()[i] += center;
        }
    }
}

void TaskPickParticle::setForces()
{

}

void TaskPickParticle::findClosestParticle()
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
    for(auto const& it : ps.positions())
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

void TaskPickParticle::doWork() 
{
    // only work on first step in case of several steps per frame
    if (gEnv->stateSim->dtActualStepPerFrame != 0) return;

    ParticleSystem& ps = particleSystem(m_workOnPsIdx);
    m_closestParticleIdx = -1;

    if (m_onClickWhatToDo == 0 || m_onClickWhatToDo == 3 || m_onClickWhatToDo == 4 || m_onClickWhatToDo == 5)
    {
        findClosestParticle();
    }

    const bool is2d = gEnv->camera->is2d();
    const bool is3d = gEnv->camera->is3d();

    if (gEnv->stateGui->mouseButtonLeftClick)
    {
        m_clickPos = gEnv->stateGui->mousePos;

        switch (m_onClickWhatToDo)
        {
        case 0: // select
        case 3: // move closest
        case 4: // move all static as closest
        case 5: // move all as closest
            m_selectedParticleIdx = m_closestParticleIdx;
            break;
        case 1: // create
            if (is2d) { ps.add(glm::vec3(gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos), 0.0f)); }
            if (is3d) { ps.add(gEnv->camera->SSToWorld3d(gEnv->stateGui->mousePos, glm::vec3(0.0f))); }
            m_selectedParticleIdx = (int)ps.size() - 1;
            break;
        default:
            break;
        }
    }

    // Rotating static particles?
    if (m_onClickWhatToDo == 6 && gEnv->stateGui->mouseButtonLeftPressed)
    {
        rotateAllStaticParticlesAroundCenter();
    }

    // Set Moved Particle to static
    if (m_onClickWhatToDo == 2 || m_onClickWhatToDo == 3)
    {
        if (gEnv->stateGui->mouseButtonLeftClick)
        {
            if (isValidParticleIdx(m_selectedParticleIdx))
            {
                m_selectedParticleStateOld = ps.particle(m_selectedParticleIdx).isStatic();
                ps.particle(m_selectedParticleIdx).setStatic(true);
            }
        }
        if (gEnv->stateGui->mouseButtonLeftRelease)
        {
            if (isValidParticleIdx(m_selectedParticleIdx))
            {
                ps.particle(m_selectedParticleIdx).setStatic(m_selectedParticleStateOld);
            }
        }
    }

    // Move Particle
    if (gEnv->stateGui->mouseButtonLeftClick || gEnv->stateGui->mouseButtonLeftPressed)
    {
        if (isValidParticleIdx(m_selectedParticleIdx))
        {
            if (m_onClickWhatToDo == 2 || m_onClickWhatToDo == 3)
            {
                if (is2d) { ps.positions()[m_selectedParticleIdx] = glm::vec3(gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos), 0.0f); }
                if (is3d) { ps.positions()[m_selectedParticleIdx] = gEnv->camera->SSToWorld3d(gEnv->stateGui->mousePos, ps.positions()[m_selectedParticleIdx]); }
            }

            if (m_onClickWhatToDo == 4 || m_onClickWhatToDo == 5)
            {
                auto& ps = particleSystem(m_workOnPsIdx);
                bool moveAll = m_onClickWhatToDo == 5;
                if (moveAll || ps.states()[m_selectedParticleIdx].isStatic())
                {
                    glm::vec3 dx(0.0f);
                    if (is2d) { dx = glm::vec3(gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos), 0.0f) - ps.positions()[m_selectedParticleIdx]; }
                    if (is3d) { dx = gEnv->camera->SSToWorld3d(gEnv->stateGui->mousePos, ps.positions()[m_selectedParticleIdx]) - ps.positions()[m_selectedParticleIdx]; }
                    for (auto& it : ps.states())
                    {
                        if (moveAll || it.isStatic()) { it.pos() += dx; }
                    }
                }
            }
        }
    }

    // Billiard
    if (m_onClickWhatToDo == 7 && isValidParticleIdx(m_selectedParticleIdx) && gEnv->stateGui->mouseButtonLeftClick)
    {
        glm::vec3 posParticle = particleSystem(m_workOnPsIdx).positions()[m_selectedParticleIdx];
        glm::vec3 posMouse(gEnv->stateGui->mousePos, 0.0f);
        glm::vec3 posMouseWS(gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos), 0);
        glm::vec3 dx = posParticle - posMouseWS;
        float scale = 4.0f;
        particleSystem(m_workOnPsIdx).velocities()[m_selectedParticleIdx] = dx*scale;
    }
}

void TaskPickParticle::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs()-1);

    ImGui::Separator();
    ImGui::RadioButton(paramName("onClick Select"), &m_onClickWhatToDo, 0);
    ImGui::RadioButton(paramName("onClick Create"), &m_onClickWhatToDo, 1);
    ImGui::RadioButton(paramName("Move Selected"), &m_onClickWhatToDo, 2);
    ImGui::RadioButton(paramName("Move Closest"), &m_onClickWhatToDo, 3);
    ImGui::RadioButton(paramName("Move all static as closest"), &m_onClickWhatToDo, 4);
    ImGui::RadioButton(paramName("Move all as closest"), &m_onClickWhatToDo, 5);
    ImGui::RadioButton(paramName("Rotate static particles"), &m_onClickWhatToDo, 6);
    ImGui::RadioButton(paramName("Set Velocity (Billiard)"), &m_onClickWhatToDo, 7);

    ImGui::Separator();
    
    // Show changeble states of selected particle
    if (isValidParticleIdx(m_selectedParticleIdx))
    {
        auto& ps = particleSystem(m_workOnPsIdx);
        const int idx = m_selectedParticleIdx;
        ImGui::SliderFloat3(paramName("Pos"), &ps.positions()[idx][0], -10.0f, 10.0f);
        ImGui::SliderFloat3(paramName("Vel"), &ps.velocities()[idx][0], -10.0f, 10.0f);
        ImGui::Text("%s %s", "Force", glm::to_string(ps.forces()[idx]).c_str());
        ImGui::ColorEdit4(paramName("Color"), &ps.colors()[idx][0]);

        ImGui::SliderFloat(paramName("Mass"), &ps.mass()[idx], -10.0f, 10.0f);

        bool bIsStatic = ps.states()[idx].isStatic();
        ImGui::Checkbox(paramName("IsStatic"), &bIsStatic);
        ps.states()[idx].setStatic(bIsStatic);

        ImGui::Separator();
        ImGui::Text("Selected Particle");
        ImGui::Text("%s", ps.states()[idx].toString().c_str());
    }
}

const char* TaskPickParticle::toString() const
{
    std::stringstream ss;
    ss << "Closest particle to mouse\n";

    ParticleSystem const& ps = particleSystem(m_workOnPsIdx);
    if (ps.isValidIdx(m_closestParticleIdx))
    {
        const int& idx = m_closestParticleIdx;

        ss << ps.states()[idx].toString() << "\n";
    }

    ss << "------------------------\n";
    ss << "Move all static as closest:\n";
    ss << "  Closest particle MUST be a static particle,\n";
    ss << "  to move all static particles.\n";

    m_string = ss.str();
    return m_string.c_str();
}

void TaskPickParticle::draw() const
{
    if (isValidParticleIdx(m_closestParticleIdx))
    {
        glm::vec3 posParticle = particleSystem(m_workOnPsIdx).positions()[m_closestParticleIdx];

        glPointSize(14.0f);
        glColor4f(1, 0, 0, 1);
        glBegin(GL_POINTS);
        glVertex3fv(glm::value_ptr(posParticle));
        glEnd();
        glPointSize(12.0f);
    }

    if (isValidParticleIdx(m_selectedParticleIdx))
    {
        glm::vec3 posParticle = particleSystem(m_workOnPsIdx).positions()[m_selectedParticleIdx];

        glPointSize(14.0f);
        glColor4f(0, 1, 0, 1);
        glBegin(GL_POINTS);
        glVertex3fv(glm::value_ptr(posParticle));
        glEnd();
        glPointSize(12.0f);
    }

    // Billiard
    if (m_onClickWhatToDo == 7 && isValidParticleIdx(m_selectedParticleIdx))
    {
        glm::vec3 posParticle = particleSystem(m_workOnPsIdx).positions()[m_selectedParticleIdx];
        glm::vec3 posMouse(gEnv->stateGui->mousePos, 0.0f);
        glm::vec3 posMouseWS(gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos),0);
        glm::vec3 dx = posParticle - posMouseWS;
        glm::vec3 to = posParticle + dx;

        glColor4f(1, 1, 1, 1);
        glBegin(GL_LINES);
        glVertex3fv(glm::value_ptr(posParticle));
        glVertex3fv(glm::value_ptr(to));
        glEnd();
    }
}
