#pragma once

#include <glm\glm.hpp>

void printVec2(glm::vec2 v);
void printVec3(glm::vec3 v);
void printVec2Array(glm::vec2 *v, int size);
void printVec3Array(glm::vec3 *v, int size);
glm::vec3 rotateAboutAny(glm::vec3 vector, glm::vec3 axis, float angle);