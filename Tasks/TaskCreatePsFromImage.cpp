#include "TaskCreatePsFromImage.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

#include "../Helper/Image.h"

void TaskCreatePsFromImage::createPsFromImage(std::string const& filename)
{
    auto& ps = particleSystem(m_workOnPsIdx);

    std::string filepath = std::string(Globals::PATH_DATA) + filename;
    std::cout << "Loading image: " << filepath << "...\n";

    Image image(filepath);

    if (image.size() == 0)
    {
        std::cout << "Error loading image: " << filepath << "\n";
        return;
    }

    glm::vec3 offs(0, 0, 0);
    float scaleTo1 = 1.0f/(float)std::max(image.width(), image.height());
    float scale = m_scale * scaleTo1;

    switch (m_createPos)
    {
    case 0: 
        offs.x -= 0.5f * scale * (float)image.width();
        break;
    case 1:
        offs.x -= 0.5f * scale * (float)image.width();
        offs.y -= 0.5f * scale * (float)image.height();
        break;
    default:
        break;
    }

    for (int y = 0; y < image.height(); y++)
        for (int x = 0; x < image.width(); x++)
        {
            glm::vec3 const& c = image.at(x, y);
            if (c.r + c.g + c.b > 0.01f) // not black?
            {
                glm::vec3 p((float)x* scale, (float)y* scale, 0.0f);
                ps.add(p+offs, glm::vec4(c,1.0f));
            }
        }
}


void TaskCreatePsFromImage::setForces()
{

}

void TaskCreatePsFromImage::doWork() 
{

}

void TaskCreatePsFromImage::draw() const
{

}

void TaskCreatePsFromImage::imGui()
{
    ImGui::SliderInt(paramName("Work on PS Idx"), &m_workOnPsIdx, 0, noOfPs() - 1);
    ImGui::Separator();
    ImGui::RadioButton(paramName("Create centered on floor"), &m_createPos, 0);
    ImGui::RadioButton(paramName("Create centered origin"), &m_createPos, 1);
    ImGui::Separator();
    ImGui::SliderFloat(paramName("Image scale"), &m_scale, 1.0f, 10.0f);
    
    ImGui::Separator();
    if (ImGui::Button(paramName("Create from black/white"))) { createPsFromImage("image_bw.png"); }
    if (ImGui::Button(paramName("Create from color"))) { createPsFromImage("image_color.png"); }

}

const char* TaskCreatePsFromImage::toString() const
{
    std::stringstream ss;
    ss << "Description:" << "\n";
    ss << "Creates particles from 'image_bw.png' or 'image_color.png'.\n";
    ss << "Creates particles only for non black pixels." << "\n";

    m_string = ss.str();
    return m_string.c_str();
}


