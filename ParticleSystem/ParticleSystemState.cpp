#include "ParticleSystemState.h"

#include <sstream>

#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include "../Globals.h"
#include "../Context/GlobalEnvironment.h"
#include "../Helper/HelperImGui.h"

ParticleSystemState::ParticleSystemState()
{

}

void ParticleSystemState::update()
{
    switch (m_updateStyle)
    {
    default:
    case 0: break;
    case 1: break;
    case 2: // read from filestream
        m_readFromFilestream.read(ps);
        break;
    case 3: // read from filestream in range
        m_readFromFilestream.read(ps);
        break;
    case 4:  // read from clipboard/single-capture file
        ps.load(Helper::cFilenameCapture);
        break;
    }
}

void ParticleSystemState::drawPsWithCurrentStyle() const
{
    if (!m_bDraw)
    {
        return;
    }

    float pointSize = m_bOverrideGlobalDrawSettings ? m_psPointSize : gEnv->stateGui->psPointSize;

    if (m_bDrawVelocities)
    {
        glColor4f(0, 1, 0, 1);
        ps.drawSlowLinesPointAndVector(ps.positions(), ps.velocities(), gEnv->stateGui->psVectorScale);
    }

    if (m_bDrawForces)
    {
        glColor4f(0, 0, 1, 1);
        ps.drawSlowLinesPointAndVector(ps.positions(), ps.forces(), gEnv->stateGui->psVectorScale);
    }

    if (m_bDrawNeighbors)
    {
        glColor4f(1, 1, 0, 1);
        int s = (int)ps.size();
        for (int i = 0; i < s; i++)
        {
            ps.states()[i].neighborList().draw(i, ps.positions());
        }
    }

    if (m_bDrawPoints)
    {
        if (m_bForceSingleColor)
        {
            ps.draw(pointSize, m_forceColor);
        }
        else
        {
            glPointSize(pointSize);
            ps.draw();
        }
    }

    glColor4f(1, 1, 1, 1);
    glPointSize(1.0f);
}

void ParticleSystemState::imGui(std::string const& pre)
{
    if (ImGui::CollapsingHeader(pre.c_str()))
    {
        ImGui::Indent();

        if (Globals::GUI_ENABLE_FILE_STREAM_FOR_PS)
        {

            int updateStyleOld = m_updateStyle;
            // Same Parameter 'm_updateStyle' -> 2 different selection styles
            ImGui::Combo((pre + "Data Input").c_str(), &m_updateStyle, "none\0none - but load once from file single capture\0from file loop\0from file loop range\0from single-capture file cont.\0\0");
            ImGui::SliderInt((pre + "Data Input As ID").c_str(), &m_updateStyle, 0, 4);

            ImGui::Separator();

            // single capture load
            if (m_updateStyle == 1)
            {
                if (m_updateStyle != updateStyleOld)
                {
                    ps.load(Helper::cFilenameCapture);
                }
            }
            // file streaming
            if (m_updateStyle == 2)
            {
                m_readFromFilestream.setMinIdx(0);
                m_readFromFilestream.setMaxIdx(999999);
            }
            // file streaming loop range
            if (m_updateStyle == 3)
            {
                // update max range on every selection
                if (m_updateStyle != updateStyleOld)
                {
                    m_fileLoopRangeMax = m_readFromFilestream.getLastFileIdx();
                }
            }
            ImGui::SliderInt((pre + "Range Min").c_str(), &m_fileLoopRangeActualStart, m_fileLoopRangeMin, m_fileLoopRangeMax);
            ImGui::SliderInt((pre + "Range Max").c_str(), &m_fileLoopRangeActualEnd, m_fileLoopRangeMin, m_fileLoopRangeMax);

            ImGui::Checkbox((pre + "Fix Range Min/Max").c_str(), &m_bFixRangeMinMax);
            if (m_bFixRangeMinMax) { m_fileLoopRangeActualEnd = m_fileLoopRangeActualStart; }
            m_readFromFilestream.setMinIdx(m_fileLoopRangeActualStart);
            m_readFromFilestream.setMaxIdx(m_fileLoopRangeActualEnd);

            ImGui::Separator();
        }
        
        ImGui::Checkbox((pre + "Simulate").c_str(), &m_bSimulate);
        ImGui::Separator();

        ImGui::Checkbox((pre + "Draw").c_str(), &m_bDraw);
        ImGui::Checkbox((pre + "DrawPoints").c_str(), &m_bDrawPoints);
        ImGui::Checkbox((pre + "DrawVelocities").c_str(), &m_bDrawVelocities);
        ImGui::Checkbox((pre + "DrawForces").c_str(), &m_bDrawForces);
        ImGui::Checkbox((pre + "DrawNeighbors").c_str(), &m_bDrawNeighbors);

        ImGui::Separator();

        ImGui::Checkbox((pre + "bOverrideGlobalDrawSettings").c_str(), &m_bOverrideGlobalDrawSettings);
        if (m_bOverrideGlobalDrawSettings)
        {
            Helper::ImGuiParticleSizeParameter(pre, m_psPointSize);
            ImGui::Separator();
        }

        ImGui::Separator();
        ImGui::Checkbox((pre + "Force Single Color").c_str(), &m_bForceSingleColor);
        if (m_bForceSingleColor)
        {
            ImGui::ColorEdit4((pre + "Force Single Color").c_str(), glm::value_ptr(m_forceColor));
        }

        ImGui::Separator();
        ImGui::Checkbox((pre + "Show PS Info").c_str(), &m_bShowPsInfo);
        if (m_bShowPsInfo)
        {
            ImGui::Text("%s", ps.toString().c_str());
        }

        ImGui::Separator();
        if (ImGui::Button((pre + "Clear PS").c_str())) { ps.clear(); }
        if (ImGui::Button((pre + "Clear Velocities").c_str())) { ps.setAllVelocitiesAndForcesToZero(); }
        if (ImGui::Button((pre + "Load Capture").c_str())) { ps.load(Helper::cFilenameCapture); }
        if (ImGui::Button((pre + "Save Capture").c_str())) { ps.save(Helper::cFilenameCapture); }

        ImGui::Separator();
        ImGui::Text("Info - No of Particles: %i", (int)ps.size());

        ImGui::Unindent();
    }
}

std::string ParticleSystemState::toString() const
{
    std::stringstream ss;
    ss << "---\n";
    ss << "m_bDraw:                    " << m_bDraw << "\n";
    ss << "m_updateStyle:              " << m_updateStyle << "\n";
    ss << "m_fileLoopRangeMin:         " << m_fileLoopRangeMin << "\n";
    ss << "m_fileLoopRangeMax:         " << m_fileLoopRangeMax << "\n";
    ss << "m_fileLoopRangeActualStart: " << m_fileLoopRangeActualStart << "\n";
    ss << "m_fileLoopRangeActualEnd:   " << m_fileLoopRangeActualEnd << "\n";
    ss << "m_bFixRangeMinMax:          " << m_bFixRangeMinMax << "\n";
    ss << "----------------------------------\n";
    ss << "ParticleSystem\n\n";
    ss << ps.toString();

    return ss.str();
}

