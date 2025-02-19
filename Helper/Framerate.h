#pragma once

#include <chrono>

class Framerate
{
	std::chrono::system_clock::time_point m_timeReference;

public:

	Framerate() = default;

	double getTimeSinceLastCall();

};