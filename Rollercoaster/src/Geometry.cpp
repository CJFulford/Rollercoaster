#include "Header.h"

std::vector<glm::vec3> controlPoints;
std::vector<float> knots;

void bSpline(int k, std::vector<glm::vec3> &output);
float deBoor_Cox(int knot, int order, float u);

void generateTrackString(std::vector<glm::vec3> &vertices)
{
	controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.f));
	controlPoints.push_back(glm::vec3( 0.5f, -0.5f, 0.f));
	controlPoints.push_back(glm::vec3( 0.5f,  0.5f, 0.f));
	controlPoints.push_back(glm::vec3(-0.5f,  0.5f, 0.f));
	//controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.f));

	bSpline(3, vertices);

    printVecVector(vertices);
}

// k is the order
void bSpline(int k, std::vector<glm::vec3> &output)
{
	// knot values need to rane from 0 to 1

	int minNumOfKnots = controlPoints.size() + k;

	for (int i = 0; i < minNumOfKnots; i++)
	{
		if (i < k)
			knots.push_back(0.f);
		else if (i > controlPoints.size())
			knots.push_back(1.f);
		else
			knots.push_back(knots[i - 1] + (1.f / (controlPoints.size() - k + 1)));
	}

	for (float u = 0.f; u < 1.f; u += 0.005f)
	{
		glm::vec3 p = glm::vec3(0.f, 0.f, 0.f);
        for (int i = 0; i < controlPoints.size(); i++)
            p += (controlPoints[i] * deBoor_Cox(i, k, u));
		output.push_back(p);
	}
}

float deBoor_Cox(int i, int order, float u)
{
	if (order == 1)
		if (knots[i] <= u && u <= knots[i + 1])
			return 1.f;
		else
			return 0.f;
	else
	{
		float ratio1 = 0.f, ratio2 = 0.f;

		// if denominator of the ratios is zero, leave that side of the equation as 0
		if (knots[i + order - 1] - knots[i] >= 0.0001f)
		{
			ratio1 = (u - knots[i]) / (knots[i + order - 1] - knots[i]);
			ratio1 *= deBoor_Cox(i, order - 1, u);
		}


		if (knots[i + order] - knots[i + 1] >= 0.0001f)
		{
			ratio2 = (knots[i + order] - u) / (knots[i + order] - knots[i + 1]);
			ratio2 *= deBoor_Cox(i + 1, order - 1, u);
		}
		return ratio1 + ratio2;
	}
}