#include "Header.h"
#include <fstream>
#include <sstream>


#define trackOrder 8 // silky smooth

using namespace std;
using namespace glm;

vector<vec3> controlPoints;
vector<float> knots;

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
        if (knots[i + order - 1] - knots[i] != 0.f)
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

float bSpline(int order, vector<vec3> &output)
{
    float maxHeight = 0.f;
    int minNumOfKnots = controlPoints.size() + order;

    for (int i = 0; i < 1.f * minNumOfKnots; i++)
    {
        if (i < order)
            knots.push_back(0.f);
        else if ((unsigned int)i > controlPoints.size())
            knots.push_back(1.f);
        else
            knots.push_back(knots[i - 1] + (1.f / (controlPoints.size() - order + 1)));
    }

    float dist = 0.05f;

    for (float u = 0.f; u < 1.f; u += 0.00001f)
    {
        vec3 point = vec3(0.f, 0.f, 0.f);
        for (int i = 0; (unsigned int)i < controlPoints.size(); i++)
            point += (controlPoints[i] * deBoor_Cox(i, order, u));

        if (u == 0.f)
        {
            output.push_back(point);
            maxHeight = max(maxHeight, point.y);
        }
        if (distance(output.back(), point) > dist)
        {
            output.push_back(point);
            maxHeight = max(maxHeight, point.y);
        }
    }
    output.push_back(output[0]);

    
    return maxHeight;
}

float generateTrackCurve(vector<vec3> &vertices)
{
    ifstream fileStream("src/controlPoints.txt");

    if (!fileStream.is_open()) cout << "Failed to open control point file" << endl;

    std::string line;
    while (std::getline(fileStream, line))
    {
        if (line[0] == '/' || line.empty()) continue;

        std::string a, b, c;

        a = line.substr(0, line.find_first_of(' '));

        line = line.substr(line.find_first_of(' '));
        line = line.substr(line.find_first_not_of(' '));

        b = line.substr(0, line.find_first_of(' '));

        line = line.substr(line.find_first_of(' '));
        line = line.substr(line.find_first_not_of(' '));

        c = line.substr(0, line.find_first_of(' '));

        controlPoints.push_back(vec3(stof(a), stof(b), stof(c)));
    }

    fileStream.close();

	return bSpline(trackOrder, vertices);

}

void generateTrackTangents(vector<vec3> &vertices, vector<vec3> &tangents)
{
    tangents.push_back(vertices.front() - vertices.back());
    for (int i = 0; (unsigned int)i < vertices.size(); i++)
        tangents.push_back(vertices[(i + 1) % vertices.size()] - vertices[i]);
}