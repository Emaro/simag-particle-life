#include "TaskEmpty.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>

void TaskEmpty::setForces()
{

}

void TaskEmpty::doWork() 
{

}

void TaskEmpty::draw() const
{

}

void TaskEmpty::imGui()
{

}

const char* TaskEmpty::toString() const
{
    std::stringstream ss;
    ss << "Do nothing...\n";
    m_string = ss.str();
    return m_string.c_str();
}


