#include "TaskRecursiveTree.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>


glm::vec3 TaskRecursiveTree::rotate2D(glm::vec3 const& v, float angle)
{
    glm::vec3 vOut(0);
	vOut.x = cos(angle)*v.x - sin(angle)*v.y;
	vOut.y = sin(angle)*v.x + cos(angle)*v.y;
	return vOut;
}

void TaskRecursiveTree::addPoint(glm::vec3 const& pos)
{
    particleSystem(0).add(pos);
}

void TaskRecursiveTree::tree(int segment, float length, float lengthMax, float lengthScale, glm::vec3 const& pos, glm::vec3 const& direction, float angleLeft, float angleRight, float step)
{
    if (segment == 0) return;

	for (float i = 0.0f; i < length; i += step)
		addPoint(pos + direction * i);
 
	tree(segment - 1, length * lengthScale, lengthMax, lengthScale, pos + direction * length, rotate2D(direction, angleLeft), angleLeft, angleRight, step);
	tree(segment - 1, length * lengthScale, lengthMax, lengthScale, pos + direction * length, rotate2D(direction, angleRight), angleLeft, angleRight, step);
	tree(segment - 1, length * lengthScale, lengthMax, lengthScale, pos + direction * length, rotate2D(direction, angleRight + angleLeft), angleLeft, angleRight, step);
}


void TaskRecursiveTree::setForces()
{

}

void TaskRecursiveTree::doWork() 
{
    if (!refreshTree) return;

    auto& ps = particleSystem(0);
    ps.clear();

    tree(segments, length, length, lengthScale, startPos, startDirection, angleLeft, angleRight, step);
}

void TaskRecursiveTree::draw() const
{

}

void TaskRecursiveTree::imGui()
{
    ImGui::Checkbox(paramName("refresh tree per frame"), &refreshTree);
    ImGui::SliderInt(paramName("segments"), &segments, 1, 10);
    ImGui::SliderFloat(paramName("length"), &length, 0.0f, 1.0f);
    ImGui::SliderFloat(paramName("lengthScale "), &lengthScale, 0.1f, 0.9f);

    //glm::vec3 startDirection = glm::vec3(0, 1, 0);
    ImGui::SliderFloat(paramName("startDirection.x"), &startDirection.x, -1, 1);
    startDirection.z = 0;
    startDirection = glm::normalize(startDirection);
    startPos = glm::vec3(0);

    ImGui::SliderFloat(paramName("angleLeft"), &angleLeft, -pi, pi);
    ImGui::SliderFloat(paramName("angleRight"), &angleRight, -pi, pi);
    ImGui::SliderFloat(paramName("step"), &step, 0.01f, 0.1f);
    if (step < 0.0005f) step = 0.009f;
}

const char* TaskRecursiveTree::toString() const
{
    std::stringstream ss;
    ss << "Render recursive tree.\n";
    ss << "Using simple recursion.\n";
    m_string = ss.str();
    return m_string.c_str();
}
