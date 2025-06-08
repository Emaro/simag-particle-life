#include "TaskAssignment_ParticleLife.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Context/GlobalEnvironment.h"
#include "../ParticleSystem/ParticleSystem.h"
#include "../Helper/HelperDraw.h"

// Add more headers here if you want...
#include <random>

namespace
{

    // Color identifiers
    int getType(const glm::vec3& color) {
        if (color == glm::vec3(0, 1, 0)) return 0; // green
        if (color == glm::vec3(1, 0, 0)) return 1; // red
        if (color == glm::vec3(0, 0, 1)) return 2; // blue
        if (color == glm::vec3(1, 1, 0)) return 3; // yellow
        if (color == glm::vec3(0.5f, 0.0f, 1.0f)) return 4; // purple
        return -1; // unknown
    }
}

void TaskAssignment_ParticleLife::setForces()
{
    fillInteractionTables(); //update tables

    auto& ps = particleSystem(m_workOnPsIdx);
    int s = (int)ps.size();
    if (s < 1) return;

    auto& pos = ps.positions();
    auto& vel = ps.velocities();
    auto& forces = ps.forces();
    auto& colors = ps.colors();

    for (int i = 0; i < s; i++) 
    {
        glm::vec3 &p0 = pos[i];
        glm::vec3 &v0 = vel[i];
        int type0 = getType(colors[i]);

        for (int j = 0; j < s; j++) 
        {
            if (i == j) continue;

            int type1 = getType(colors[j]);
            float radius = m_radiusTable[type0][type1];

            glm::vec3 p1 = pos[j];
            glm::vec3 diff = p1 - p0;
            
            // check if particle is in radius, if not skip
            float distSq = glm::dot(diff, diff);

            // small epsilon check, to ensure no div by 0 happens (particles exactly on eachother)
            // (which would just erase everything lol)
            if (distSq > radius * radius || distSq < 1e-6f) continue;

            float dist = std::sqrt(distSq);
            glm::vec3 dir = diff / dist;

            float f_coef = m_forceTable[type0][type1] * -1; // <0 = Attract, >0 = repel 
            float f_strength = f_coef * (1.0f - (dist / radius));
            forces[i] += dir * f_strength;
        }

        // Bounding
        if (m_bounded)
        {
            float e = 0.01f; // small epsilon
            if (p0.x > m_w_xmax) {
                p0.x = m_w_xmax - e;
                v0.x *= -1;
            }

            if (p0.x < m_w_xmin) {
                p0.x = m_w_xmin + e;
                v0.x *= -1;
            }

            if (p0.y > m_w_ymax) {
                p0.y = m_w_ymax - e;
                v0.y *= -1;
            }

            if (p0.y < m_w_ymin) {
                p0.y = m_w_ymin + e;
                v0.y *= -1;
            }
        }

        v0 *= (1.0f - m_viscosity);
    }
}

void TaskAssignment_ParticleLife::doWork() 
{
    // Anything, which is not force/velocity-related (i.e., timestep related) comes here.
    // Access of particles similiar to "setForces()".
    // This method is called every frame, not only when simulation is running.

    // E.g., change colors of all particles:
    
    // if (!m_boolTest) return;       // Early exit -> do nothing, if checkbox is not set

    auto& ps0 = particleSystem(0); // Get particle system 0 -> ps0
    size_t s = ps0.size();         // Number of particles in ps0
    if (ps0.size() == 0) return;   // Nothing to do


    // Work on particles, e.g. animate colors
    // m_count += 0.04f;
    //glm::vec4 newColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * (sinf(m_count) * 0.5f + 0.5f); // Set color according to sine (colors range from 0.0 .. 1.0)
    // newColor.a = 1.0f; // ensure alpha = 1.0
    /*for (int i = 0; i < (int)s; i++)
    {
        newColor.r = sinf(ps0.particle(i).pos().x) * 0.3f + 0.7f; // Set red channel according to x pos
        ps0.particle(i).color() = newColor;
    }*/
}

void TaskAssignment_ParticleLife::draw() const
{
    // Draw Wall
    if (m_bounded)
    {
        glColor3f(0, 1, 0);
        glLineWidth(3.0f);
        Helper::drawBox2d(m_w_xmin, m_w_ymin, m_w_xmax, m_w_ymax);
        glLineWidth(1.0f);
        glColor3f(1, 1, 1);
    }
}

void TaskAssignment_ParticleLife::generateRandomScene()
{
    auto& ps = particleSystem(m_workOnPsIdx);
    ps.clear();

    // colors
    std::vector<glm::vec4> colors = {
        glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, // Green
        glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}, // Red
        glm::vec4{0.0f, 0.0f, 1.0f, 1.0f}, // Blue
        glm::vec4{1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
        glm::vec4{0.5f, 0.0f, 1.0f, 1.0f}  // Purple
    };

    int no_particles = m_noParticles;
    
    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> dist_x(m_w_xmin, m_w_xmax);
    std::uniform_real_distribution<float> dist_y(m_w_ymin, m_w_ymax);
    std::uniform_int_distribution<int> dist_color(0, 4);

    for (int i = 0; i < no_particles; ++i) {
        glm::vec3 pos(dist_x(rng), dist_y(rng), 0.0f);
        int rng_col = dist_color(rng);
        glm::vec4 color = colors[rng_col];
        ps.add(pos, color);
    }
}

void TaskAssignment_ParticleLife::fillInteractionTables() {
    // Green interactions
    m_forceTable[0][0] = m_gxg; m_radiusTable[0][0] = m_rgxg;
    m_forceTable[0][1] = m_gxr; m_radiusTable[0][1] = m_rgxr;
    m_forceTable[0][2] = m_gxb; m_radiusTable[0][2] = m_rgxb;
    m_forceTable[0][3] = m_gxy; m_radiusTable[0][3] = m_rgxy;
    m_forceTable[0][4] = m_gxp; m_radiusTable[0][4] = m_rgxp;

    // Red interactions
    m_forceTable[1][0] = m_rxg; m_radiusTable[1][0] = m_rrxg;
    m_forceTable[1][1] = m_rxr; m_radiusTable[1][1] = m_rrxr;
    m_forceTable[1][2] = m_rxb; m_radiusTable[1][2] = m_rrxb;
    m_forceTable[1][3] = m_rxy; m_radiusTable[1][3] = m_rrxy;
    m_forceTable[1][4] = m_rxp; m_radiusTable[1][4] = m_rrxp;

    // Blue interactions
    m_forceTable[2][0] = m_bxg; m_radiusTable[2][0] = m_rbxg;
    m_forceTable[2][1] = m_bxr; m_radiusTable[2][1] = m_rbxr;
    m_forceTable[2][2] = m_bxb; m_radiusTable[2][2] = m_rbxb;
    m_forceTable[2][3] = m_bxy; m_radiusTable[2][3] = m_rbxy;
    m_forceTable[2][4] = m_bxp; m_radiusTable[2][4] = m_rbxp;

    // Yellow interactions
    m_forceTable[3][0] = m_yxg; m_radiusTable[3][0] = m_ryxg;
    m_forceTable[3][1] = m_yxr; m_radiusTable[3][1] = m_ryxr;
    m_forceTable[3][2] = m_yxb; m_radiusTable[3][2] = m_ryxb;
    m_forceTable[3][3] = m_yxy; m_radiusTable[3][3] = m_ryxy;
    m_forceTable[3][4] = m_yxp; m_radiusTable[3][4] = m_ryxp;

    // Purple interactions
    m_forceTable[4][0] = m_pxg; m_radiusTable[4][0] = m_rpxg;
    m_forceTable[4][1] = m_pxr; m_radiusTable[4][1] = m_rpxr;
    m_forceTable[4][2] = m_pxb; m_radiusTable[4][2] = m_rpxb;
    m_forceTable[4][3] = m_pxy; m_radiusTable[4][3] = m_rpxy;
    m_forceTable[4][4] = m_pxp; m_radiusTable[4][4] = m_rpxp;
}


void TaskAssignment_ParticleLife::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();

    ImGui::SliderInt(paramName("Number of particles"), &m_noParticles, 0, 10000);
    ImGui::SliderFloat("Viscosity", &m_viscosity, 0.0f, 1.0f);
    ImGui::Checkbox(paramName("Bounded?"), &m_bounded);
    float w_min = -20;
    float w_max = 20;
    if (m_bounded)
    {
        ImGui::SliderInt(paramName("Wall Left"), &m_w_xmin, w_min, 0);
        ImGui::SliderInt(paramName("Wall Right"), &m_w_xmax, 0, w_max);
        ImGui::SliderInt(paramName("Wall Down"), &m_w_ymin, w_min, 0);
        ImGui::SliderInt(paramName("Wall Up"), &m_w_ymax, 0, w_max);
    }
    ImGui::Separator();

    if (ImGui::Button(paramName("Generate Scene"))) { generateRandomScene(); }

    float f_min = -1.0f;
    float f_max = 1.0f;
    float r_min = 0.01f;
    float r_max = 5.0f;

    ImGui::Begin("Interaction Sliders");
    ImGui::Text("Force Green on X:");
    ImGui::SliderFloat("Green -> Green", &m_gxg, f_min, f_max);
    ImGui::SliderFloat("Green -> Red", &m_gxr, f_min, f_max);
    ImGui::SliderFloat("Green -> Blue", &m_gxb, f_min, f_max);
    ImGui::SliderFloat("Green -> Yellow", &m_gxy, f_min, f_max);
    ImGui::SliderFloat("Green -> Purple", &m_gxp, f_min, f_max);

    ImGui::SliderFloat("Radius Green -> Green", &m_rgxg, r_min, r_max);
    ImGui::SliderFloat("Radius Green -> Red", &m_rgxr, r_min, r_max);
    ImGui::SliderFloat("Radius Green -> Blue", &m_rgxb, r_min, r_max);
    ImGui::SliderFloat("Radius Green -> Yellow", &m_rgxy, r_min, r_max);
    ImGui::SliderFloat("Radius Green -> Purple", &m_rgxp, r_min, r_max);
    ImGui::Separator();

    ImGui::Text("Force Red on X:");
    ImGui::SliderFloat("Red -> Green", &m_rxg, f_min, f_max);
    ImGui::SliderFloat("Red -> Red", &m_rxr, f_min, f_max);
    ImGui::SliderFloat("Red -> Blue", &m_rxb, f_min, f_max);
    ImGui::SliderFloat("Red -> Yellow", &m_rxy, f_min, f_max);
    ImGui::SliderFloat("Red -> Purple", &m_rxp, f_min, f_max);

    ImGui::SliderFloat("Radius Red -> Green", &m_rrxg, r_min, r_max);
    ImGui::SliderFloat("Radius Red -> Red", &m_rrxr, r_min, r_max);
    ImGui::SliderFloat("Radius Red -> Blue", &m_rrxb, r_min, r_max);
    ImGui::SliderFloat("Radius Red -> Yellow", &m_rrxy, r_min, r_max);
    ImGui::SliderFloat("Radius Red -> Purple", &m_rrxp, r_min, r_max);
    ImGui::Separator();

    ImGui::Text("Force Blue on X:");
    ImGui::SliderFloat("Blue -> Green", &m_bxg, f_min, f_max);
    ImGui::SliderFloat("Blue -> Red", &m_bxr, f_min, f_max);
    ImGui::SliderFloat("Blue -> Blue", &m_bxb, f_min, f_max);
    ImGui::SliderFloat("Blue -> Yellow", &m_bxy, f_min, f_max);
    ImGui::SliderFloat("Blue -> Purple", &m_bxp, f_min, f_max);

    ImGui::SliderFloat("Radius Blue -> Green", &m_rbxg, r_min, r_max);
    ImGui::SliderFloat("Radius Blue -> Red", &m_rbxr, r_min, r_max);
    ImGui::SliderFloat("Radius Blue -> Blue", &m_rbxb, r_min, r_max);
    ImGui::SliderFloat("Radius Blue -> Yellow", &m_rbxy, r_min, r_max);
    ImGui::SliderFloat("Radius Blue -> Purple", &m_rbxp, r_min, r_max);
    ImGui::Separator();

    ImGui::Text("Force Yellow on X:");
    ImGui::SliderFloat("Yellow -> Green", &m_yxg, f_min, f_max);
    ImGui::SliderFloat("Yellow -> Red", &m_yxr, f_min, f_max);
    ImGui::SliderFloat("Yellow -> Blue", &m_yxb, f_min, f_max);
    ImGui::SliderFloat("Yellow -> Yellow", &m_yxy, f_min, f_max);
    ImGui::SliderFloat("Yellow -> Purple", &m_yxp, f_min, f_max);

    ImGui::SliderFloat("Radius Yellow -> Green", &m_ryxg, r_min, r_max);
    ImGui::SliderFloat("Radius Yellow -> Red", &m_ryxr, r_min, r_max);
    ImGui::SliderFloat("Radius Yellow -> Blue", &m_ryxb, r_min, r_max);
    ImGui::SliderFloat("Radius Yellow -> Yellow", &m_ryxy, r_min, r_max);
    ImGui::SliderFloat("Radius Yellow -> Purple", &m_ryxp, r_min, r_max);
    ImGui::Separator();

    ImGui::Text("Force Purple on X:");
    ImGui::SliderFloat("Purple -> Green", &m_pxg, f_min, f_max);
    ImGui::SliderFloat("Purple -> Red", &m_pxr, f_min, f_max);
    ImGui::SliderFloat("Purple -> Blue", &m_pxb, f_min, f_max);
    ImGui::SliderFloat("Purple -> Yellow", &m_pxy, f_min, f_max);
    ImGui::SliderFloat("Purple -> Purple", &m_pxp, f_min, f_max);

    ImGui::SliderFloat("Radius Purple -> Green", &m_rpxg, r_min, r_max);
    ImGui::SliderFloat("Radius Purple -> Red", &m_rpxr, r_min, r_max);
    ImGui::SliderFloat("Radius Purple -> Blue", &m_rpxb, r_min, r_max);
    ImGui::SliderFloat("Radius Purple -> Yellow", &m_rpxy, r_min, r_max);
    ImGui::SliderFloat("Radius Purple -> Purple", &m_rpxp, r_min, r_max);
    ImGui::End();
}

const char* TaskAssignment_ParticleLife::toString() const
{
    std::stringstream ss;

    ss << "Description:" << "\n";
    ss << "Particle life simulation: \n Move the sliders to attract or repel the particle types" << "\n";
    ss << "" << "\n";

    /*if (m_boolTest)
    {
        ss << "Checkbox: IS CHECKED => draw red line\n";
    }
    else
    {
        ss << "Checkbox: NOT CHECKED\n";
    }*/

    // ss << "SliderFloat Value: " << m_sliderValue << "\n";



    m_string = ss.str();
    return m_string.c_str();
}


