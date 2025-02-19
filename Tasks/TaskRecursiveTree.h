#pragma once

#include "ITask.h"

class TaskRecursiveTree : public CTask
{
public:

	TaskRecursiveTree() : CTask() {}

	// ITask
	virtual const char* name() const override { return "TaskRecursiveTree"; }
	virtual void setForces() override;
	virtual void doWork() override;
	virtual void draw() const override;
	virtual void imGui() override;
	virtual const char* toString() const override;

private:

	bool refreshTree = true;

	float pi = 3.141f;
	int segments = 7;
	float length = 1.0f;
	float lengthScale = 0.6f;
	glm::vec3 startPos = glm::vec3(0, 0, 0);
	glm::vec3 startDirection = glm::vec3(0, 1, 0);
	float angleLeft = pi / 8;
	float angleRight = -pi / 4;
	float step = 0.025f;

	glm::vec3 rotate2D(glm::vec3 const& v, float angle);

	void addPoint(glm::vec3 const& pos);

	void tree(int segment, float length, float lengthMax, float lengthScale, glm::vec3 const& pos, glm::vec3 const& direction, float angleLeft, float angleRight, float step);

};
