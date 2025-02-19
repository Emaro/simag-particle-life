#include "NeighborList.h"

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

void NeighborList::draw(int idxSelf, std::vector<glm::vec3> const& pos) const
{
	size_t s = (int)m_neighbors.size();
	if (s == 0) return;
	glm::vec3 p0 = pos[idxSelf];

	Neighbor const* n = &m_neighbors.front();
	glBegin(GL_LINES);
	for (size_t i = 0; i < s; i++)
	{
		glVertex3fv(&p0[0]);
		glVertex3fv(&pos[n->idx][0]);
		n++;
	}
	glEnd();
}

