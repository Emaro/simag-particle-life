#include "ParticleSystem.h"

#include <glad/gl.h>

void ParticleSystem::drawBuffered(glm::vec4 const& color) const
{
	if (size() == 0) return;
	glColor4fv(&color[0]);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * size(), &m_pos[0], GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_POINTS, 0, (int)m_pos.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ParticleSystem::drawBuffered(void* colorData) const
{
	if (size() == 0) return;
	glColor4f(1, 1, 1, 1);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * size(), &m_pos[0], GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * size(), colorData, GL_STATIC_DRAW);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glDrawArrays(GL_POINTS, 0, (int)m_pos.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void ParticleSystem::draw() const
{
	if (size() == 0) return;
	drawBuffered((void*)&m_col[0]);
}

void ParticleSystem::draw(float pointSize, glm::vec4 const& color) const
{
	glPointSize(pointSize);
	drawBuffered(color);
}

void ParticleSystem::drawSlowLinesPointAndVector(std::vector<glm::vec3> const& pos, std::vector<glm::vec3> const& vec, float scale) const
{
	size_t s = size();
	if (s == 0) return;

	glm::vec3 v;
	glm::vec3 const* p0 = &pos.front();
	glm::vec3 const* p1 = &vec.front();
	glBegin(GL_LINES);
	for (size_t i = 0; i < s; i++)
	{
		v = *p0 + *p1 * scale;
		glVertex3fv(&(*p0)[0]);
		glVertex3fv(&v[0]);
		p0++; p1++;
	}
	glEnd();
}

void ParticleSystem::allocateOpenGlStuff()
{
	glGenBuffers(1, &m_bufferVertex);
	glGenBuffers(1, &m_bufferColor);
}
