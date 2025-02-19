#pragma once

#include <vector>

#include <glm/vec3.hpp>

struct Neighbor
{
	Neighbor(int idx, float distance) : distance(distance), idx(idx) {}
	float distance;
	int idx;
};

class NeighborList
{

public:

	std::vector<Neighbor> const& get() const { return m_neighbors; }
	std::vector<Neighbor>& get() { return m_neighbors; }

	void push(Neighbor const& neighbor)
	{
		m_neighbors.emplace_back(neighbor);
	}

	void clear() { m_neighbors.clear(); }

	void draw(int idxSelf, std::vector<glm::vec3> const& pos) const;

private:

	std::vector<Neighbor> m_neighbors;

};

