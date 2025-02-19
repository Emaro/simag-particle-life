#include "IntegratorRK4.h"

namespace
{

	void evaluate(
		glm::vec3 const& posInitial, glm::vec3 const& velInitial,
		float const& dt,
		glm::vec3 const& derivativeInDx,
		glm::vec3 const& derivativeInDv,

		glm::vec3& newPos,
		glm::vec3& newVel
	)
	{
		newPos = posInitial + derivativeInDx * dt;
		newVel = velInitial + derivativeInDv * dt;
	}

}

void IntegratorRK4::step(
	int passNumber,
	float dt,
	glm::vec3* pos,
	glm::vec3* vel,
	glm::vec3* force,
	float* mass,
	glm::vec3* temp0,
	glm::vec3* temp1,
	glm::vec3* temp2,
	glm::vec3* temp3,
	int nSize
) const
{
	if (nSize == 0) return;

	// todo students

}
