#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>

class Image
{
	int m_width;
	int m_height;
	int m_channels;
	std::vector<glm::vec3> m_data;

public:

	Image(std::string const& filename);

	int width() const { return m_width; }
	int height() const { return m_height; }
	int channels() const { return m_channels; }
	int size() const { return m_width * m_height; }

	glm::vec3 const* data() const { return &m_data[0]; }
	glm::vec3* data() { return &m_data[0]; }

	glm::vec3 const& at(int x, int y) const { assert((x>=0) && (y>=0) && (x*y<size()) && "Idx out of bounds"); return m_data[(size_t)y * m_width + x]; }
	glm::vec3& at(int x, int y) { assert((x >= 0) && (y >= 0) && (x * y < size()) && "Idx out of bounds"); return m_data[(size_t)y * m_width + x]; }

	void print() const;
};
