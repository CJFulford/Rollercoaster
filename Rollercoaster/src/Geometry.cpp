#include "Header.h"

std::vector<glm::vec3> controlPoints, points;
std::vector<float> knots;

void bSpline(int k);
float deBoor_Cox(int knot, float position, int order);

void generateTrackString(std::vector<glm::vec3> &vertices)
{
	controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.f));
	controlPoints.push_back(glm::vec3(0.5f, -0.5f, 0.f));
	controlPoints.push_back(glm::vec3(	0.5f,	0.5f,	0.f));
	controlPoints.push_back(glm::vec3(	-0.5f,	0.5f,	0.f));
	controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.f));

	bSpline(2);

	vertices = points;

	printVecVector(points);
}

// k is the order
void bSpline(int k)
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
			knots.push_back(knots[i - 1] + (1.f / minNumOfKnots - (2 * k)));
	}


	for (float u = 0.f; u < 1.f; u += 0.02f)
	{
		glm::vec3 point(0.f, 0.5f, 0.f);
		for (int i = 0; i < controlPoints.size(); i++)
			point += controlPoints[i] * deBoor_Cox(i, k, u);
		points.push_back(point);
	}
}

float deBoor_Cox(int knot, float position, int order)
{
	if (order <= 1)
		if (knots[knot] <= position && position <= knots[knot + 1])
			return 1.f;
		else
			return 0.f;
	else
	{
		float ratio1 = 0.f, ratio2 = 0.f;

		// if denominator of the ratios is zero, leave that side of the equation as 0
		if (knots[knot + order - 1] - knots[knot] != 0.f)
		{
			ratio1 = (position - knots[knot]) / (knots[knot + order - 1] - knots[knot]);
			ratio1 *= deBoor_Cox(knot, position, order - 1);
		}


		if (knots[knot + order] - knots[knot + 1] != 0.f)
		{
			ratio2 = (knots[knot + order] - position) / (knots[knot + order] - knots[knot + 1]);
			ratio2 *= deBoor_Cox(knot, position + 1, order - 1);
		}
		return ratio1 + ratio2;
	}
}