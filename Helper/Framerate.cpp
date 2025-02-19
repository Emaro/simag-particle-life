#include "Framerate.h"

double Framerate::getTimeSinceLastCall()
{
	auto timeNow  = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

	std::chrono::duration<double> elapsed = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()) - m_timeReference;

	m_timeReference = timeNow;
	
	//double fps = 1.0 / (elapsed.count());
	return elapsed.count();
}

