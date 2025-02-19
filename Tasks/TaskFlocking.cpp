#include "TaskFlocking.h"

#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <imgui/imgui.h>
#include "glm/gtx/string_cast.hpp"

namespace
{
	const float eps = 0.0001f;

	void limitVectorLength(glm::vec3& v, float lengthMax)
	{
		float l = glm::length(v);
		if ( l > lengthMax)
		{
			v = v / l * lengthMax;  // Limit to maximum steering force
		}
	}
}

glm::vec3 TaskFlocking::steerForceCraigReynolds(int particleIdx, glm::vec3 const& direction)
{
	auto& ps = particleSystem(0);

	// Implement Reynolds Steering: 
	// SteeringForce = DesiredVelocity - Velocity
	// DesiredVelocity: m_maxSpeed into direction
	
	glm::vec3 steer(0);

	// todo students

	limitVectorLength(steer, m_maxForce);
	return steer;
}

// Separation
// Method checks for nearby boids and steers away
glm::vec3 TaskFlocking::separate(int particleIdx)
{
	auto& ps = particleSystem(0);
	if (ps.size() < 2) return glm::vec3(0);
	if (particleIdx >= (int) ps.size()) return glm::vec3(0);

	glm::vec3 steer(0, 0, 0);
	int count = 0;
	auto& pos = ps.positions()[particleIdx];

	// For every boid in the system, check if it's too close
  	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		auto& posOther = ps.positions()[i];
		float d = glm::length(pos - posOther);
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < m_desiredSeparation))
		{
			// Calculate vector pointing away from neighbor
			// Therefore, normalize the distance and weight by distance (i.e., divide by d)
			// Acuumulate in steer

			// todo students

			count++; // Keep track of how many
		}
	}

	// Average -- divide by how many
	if (count > 0)
	{
		steer /= (float)count;
	}

	// As long as the vector is greater than 0
	if (glm::length(steer) > eps)
	{
		steer = steerForceCraigReynolds(particleIdx, steer);
	}
	return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
glm::vec3 TaskFlocking::align(int particleIdx)
{
	auto& ps = particleSystem(0);
	if (ps.size() < 2) return glm::vec3(0);
	if (particleIdx >= (int)ps.size()) return glm::vec3(0);

	auto& pos = ps.positions()[particleIdx];

	glm::vec3 sum(0);
	int count = 0;
	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		auto& posOther = ps.positions()[i];
		float d = glm::length(pos - posOther);
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < m_neighborDistAlign)) 
		{
			auto& velOther = ps.velocities()[i];
			sum+=velOther;
			count++;
		}
	}

	if (count > 0) 
	{
		sum/=(float)count;
		if (glm::length(sum) < eps) return glm::vec3(0); // avoid division by zero
		glm::vec3 steer = glm::normalize(sum);
		return steerForceCraigReynolds(particleIdx, steer);

	}
	else 
	{
		return glm::vec3(0);
	}
}

// Cohesion
// For the average position (i.e. center) of all nearby boids, calculate steering vector towards that position
glm::vec3 TaskFlocking::cohesion(int particleIdx)
{
	auto& ps = particleSystem(0);
	if (ps.size() < 2) return glm::vec3(0);
	if (particleIdx >= (int)ps.size()) return glm::vec3(0);

	auto& pos = ps.positions()[particleIdx];

	glm::vec3 sum(0); // Start with empty vector to accumulate all positions
	int count = 0;
	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		auto& posOther = ps.positions()[i];
		float d = glm::length(pos - posOther);
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < m_neighborDistCohesion))
		{
			sum+=posOther;
			count++;
		}
	}

	if (count > 0) 
	{
		sum/=(float)count;
		glm::vec3 desired = sum - ps.positions()[particleIdx];  // A vector pointing from the position to the target
		return steerForceCraigReynolds(particleIdx, desired);
	}
	else 
	{
		return glm::vec3(0);
	}
}

void TaskFlocking::handleBorders()
{
	auto& ps = particleSystem(0);
	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		auto& position = ps.positions()[i];
		if (position.x < -m_border) position.x = m_border;
		if (position.y < -m_border) position.y = m_border;
		if (position.z < -m_border) position.z = m_border;

		if (position.x > m_border) position.x = -m_border;
		if (position.y > m_border) position.y = -m_border;
		if (position.z > m_border) position.z = -m_border;
	}
}

void TaskFlocking::handle2D()
{
	auto& ps = particleSystem(0);
	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		ps.positions()[i].z = 0;
	}
}

void TaskFlocking::setForces()
{
	auto& ps = particleSystem(0);
	if (ps.size() < 2) return;

	handleBorders();
	if (m_force2D) handle2D();

	for (int i = (int)ps.size() - 1; i >= 0; i--)
	{
		auto& velocity = ps.velocities()[i];

		glm::vec3 sep = separate(i);   // Separation force
		glm::vec3 ali = align(i);      // Alignment force
		glm::vec3 coh = cohesion(i);   // Cohesion force

		// Weight these forces
		sep *= m_forceSeparation; 
		ali *= m_forceAlign;
		coh *= m_forceCohesion;
		
		// Add the forces
		glm::vec3 force = sep + ali + coh;
		force *= m_forceIntensity;

		limitVectorLength(velocity, m_maxSpeed);
		limitVectorLength(force, m_maxForce);
		ps.forces()[i] += force;
	}
}

void TaskFlocking::doWork()
{

}

void TaskFlocking::draw() const
{

}

void TaskFlocking::imGui()
{
	ImGui::Checkbox("Force 2D##Flocking", &m_force2D);
	ImGui::SliderFloat("Force Intensity Scale##Flocking", &m_forceIntensity, 0.1f, 10.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Max Speed##Flocking", &m_maxSpeed, 1.0f, 10.0f);
	ImGui::SliderFloat("Max Force##Flocking", &m_maxForce, 1.0f, 20.0f);

	ImGui::Separator();
	ImGui::SliderFloat("Separation Force Scale##Flocking", &m_forceSeparation, 0.2f, 4.0f);
	ImGui::SliderFloat("Separation Desired Dist##Flocking", &m_desiredSeparation, 0.05f, 0.5f);

	ImGui::SliderFloat("Align Force Scale##Flocking", &m_forceAlign, 0.2f, 4.0f);
	ImGui::SliderFloat("Align Neighbor Dist##Flocking", &m_neighborDistAlign, 0.05f, 0.5f);

	ImGui::SliderFloat("Cohesion Force Scale##Flocking", &m_forceCohesion, 0.2f, 4.0f);
	ImGui::SliderFloat("Cohesion Neighbor Dist##Flocking", &m_neighborDistCohesion, 0.05f, 0.5f);
}

const char* TaskFlocking::toString() const
{
	std::stringstream ss;
	ss << "Description:\nFlocking behaviour of particles.\n";
	ss << "Increase MaxSpeed and SeparationForce for good results.\n";
	ss << "Force 2D: Sets z-coordinate to 0 on each frame\n";
	m_string = ss.str();
	return m_string.c_str();
}


