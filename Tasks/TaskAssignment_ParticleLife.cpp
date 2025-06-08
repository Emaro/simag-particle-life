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

    // Add your local functions here...

}

void TaskAssignment_ParticleLife::setForces()
{
    // Set forces & velocities of particles here...
    // E.g.:

    /*
    auto& ps0 = particleSystem(0); // Get particle system 0 -> ps0
    size_t s = ps0.size();         // Number of particles in ps0
    if (ps0.size() == 0) return;   // Nothing to do

    // Work on particles, e.g. set all velocities to zero (0,0,0)
    for (int i = 0; i < s; i++)
    {
        ps0.particle(i).vel() = glm::vec3(0.0f, 0.0f, 0.0f);
        // ps0.particle(i).force() = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    */
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
    /*if (m_boolTest)
    {
        // Draw diagonal line in red
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glLineWidth(5.0f);
        Helper::drawLine2d(-5.0f, -5.0f, 5.0f, 5.0f);
        glLineWidth(1.0f);
    }*/
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
    std::uniform_real_distribution<float> dist_pos(-5.0f, 5.0f);
    std::uniform_int_distribution<int> dist_color(0, 4);

    for (int i = 0; i < no_particles; ++i) {
        glm::vec3 pos(dist_pos(rng), dist_pos(rng), 0.0f);
        int rng_col = dist_color(rng);
        glm::vec4 color = colors[rng_col];
        ps.add(pos, color);
    }
}

void TaskAssignment_ParticleLife::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();

    ImGui::SliderInt(paramName("Numbe of particles"), &m_noParticles, 0, 10000);
    ImGui::Separator();

    if (ImGui::Button(paramName("Generate Scene"))) { generateRandomScene(); }

    float f_min = -1.0f;
    float f_max = 1.0f;
    float r_min = 0.01f;
    float r_max = 5.0f;

    ImGui::Begin("Interaction Sliders");
    ImGui::Text("Green on others:");
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

    ImGui::Text("Red on others:");
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

    ImGui::Text("Blue on others:");
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

    ImGui::Text("Yellow on others:");
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

    ImGui::Text("Purple on others:");
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


