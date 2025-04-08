#include "IntegratorRK2_Midpoint.h"

void IntegratorRK2_Midpoint::step(
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


	if (passNumber == 0)
	{
		for (int i = 0; i < nSize; i++)
		{
			auto& p = *pos;
			auto& v = *vel;
			auto& m = *mass;
			auto& f = *force;
			auto& t0 = *temp0;
			auto& t1 = *temp1;

			// Use t0 and t1 as temporary values, which can be used in pass 2
			

			// velocity-update
			t1 = v;
			v = v + (dt/2) * f / m;

			t0 = p;
			p = p + (dt/2) * t1;
		

			pos++;
			vel++;
			force++;
			mass++;
			temp0++;
			temp1++;
		}
	}

	if (passNumber == 1)
	{
		for (int i = 0; i < nSize; i++)
		{
			auto& p = *pos;
			auto& v = *vel;
			auto& m = *mass;
			auto& f = *force;
			auto& t0 = *temp0;
			auto& t1 = *temp1;

			// todo students

			p = t0 + dt * v;
		
			// velocity-update
			v = t1 + dt * f / m;

			pos++;
			vel++;
			force++;
			mass++;
			temp0++;
			temp1++;
		}
	}
}
