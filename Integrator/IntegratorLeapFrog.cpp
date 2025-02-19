#include "IntegratorLeapFrog.h"

void IntegratorLeapFrog::step(
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
