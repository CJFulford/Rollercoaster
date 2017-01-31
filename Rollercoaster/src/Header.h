#pragma once

#include "Tools.h"

#include <glm\glm.hpp>
#include <vector>

const float PI = 3.14159265359f,
			twoPI = 2.f * PI;

const int WINDOW_SIZE = 500;

void generateTrackString(std::vector<glm::vec3> &vertices);