#pragma once

#include "Tools.h"

#include <glm\glm.hpp>
#include <vector>

const float PI = 3.14159265359f,
            twoPI = 2.f * PI,
            halfPI = PI / 2.f;

const int WINDOW_SIZE = 500;

const glm::mat4 identity(1.0f);

const glm::vec3 center(0.f, 0.f, 0.f),
                up(0.f, 1.f, 0.f),
                cam(0.f, 0.5f, 2.f);

void generateTrackString(std::vector<glm::vec3> &vertices);