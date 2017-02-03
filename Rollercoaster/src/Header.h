#pragma once

#include "Tools.h"

#include <glm\glm.hpp>
#include <vector>

const float PI = 3.14159265359f,
            twoPI = 2.f * PI,
            halfPI = PI / 2.f,
            gravity = 9.81f;

const int WINDOW_WIDTH = 700,
            WINDOW_HEIGHT = 500;

const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

const glm::mat4 identity(1.0f);

const glm::vec3 up(0.f, 1.f, 0.f),
                cam(0.f, 0.5f, 2.f);

float generateTrackCurve(std::vector<glm::vec3> &vertices);
void generateTrackTangents(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &tangents);