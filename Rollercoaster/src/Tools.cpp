#include "Tools.h"
#include <iostream>

void printVec2(glm::vec2 v)
{
	std::cout << v.x << "\t" << v.y << std::endl;
}

void printVec3(glm::vec3 v)
{
	std::cout << v.x << "\t" << v.y << "\t" << v.z <<  std::endl;
}

void printVec2Array(glm::vec2 *v, int size)
{
	for (int i = 0; i < size; i++)
		printVec2(v[i]);
}

void printVec3Array(glm::vec3 *v, int size)
{
	for (int i = 0; i < size; i++)
		printVec3(v[i]);
}

//Rodrigues' rotation formula
glm::vec3 rotateAboutAny(glm::vec3 vector, glm::vec3 axis, float angle)
{
	return glm::vec3((vector * cos(angle)) + 
					(glm::cross(axis, vector) * sin(angle)) + 
					(axis * glm::dot(axis, vector) * (1.f - cos(angle))));
}

