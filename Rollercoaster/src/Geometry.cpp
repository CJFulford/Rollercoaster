#include "Header.h"
#include <fstream>
#include <sstream>

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

void bSpline(int order, vector<vec3> &output)
{
    int minNumOfKnots = controlPoints.size() + order;

    for (int i = 0; i < minNumOfKnots; i++)
    {
        if (i < order)
            knots.push_back(0.f);
        else if ((unsigned int)i > controlPoints.size())
            knots.push_back(1.f);
        else
            knots.push_back(knots[i - 1] + (1.f / (controlPoints.size() - order + 1)));
    }

    for (float u = 0.f; u < 1.f; u += 0.0001f)
    {
        vec3 point = vec3(0.f, 0.f, 0.f);
        for (int i = 0; (unsigned int)i < controlPoints.size(); i++)
            point += (controlPoints[i] * deBoor_Cox(i, order, u));
        output.push_back(point);
    }
}

void generateTrackCurve(vector<vec3> &vertices)
{

    ifstream fileStream("controlPoints.txt");

    std::string line;
    while (std::getline(fileStream, line))
    {
        if (line[0] == '/' || line[0] == '\n') continue;

        float x, y, z;
        std::string a, b, c;

        a = line.substr(0, line.find_first_of(' '));

        line = line.substr(line.find_first_of(' '));
        line = line.substr(line.find_first_not_of(' '));

        b = line.substr(0, line.find_first_of(' '));

        line = line.substr(line.find_first_of(' '));
        line = line.substr(line.find_first_not_of(' '));

        c = line.substr(0, line.find_first_of(' '));

        x = stof(a);
        y = stof(b);
        z = stof(c);

        controlPoints.push_back(vec3(x, y, z));


    }

    fileStream.close();




    /*// the chain lift and initial drop
	controlPoints.push_back(vec3(-0.75f, 0.f, 0.25f));
	controlPoints.push_back(vec3(-0.5f, 0.f, 0.25f));
	controlPoints.push_back(vec3( 0.f, 0.7f, 0.25f));
    controlPoints.push_back(vec3(0.5f, 0.f, 0.25f));
    controlPoints.push_back(vec3(0.75f, 0.f, 0.25f));
    controlPoints.push_back(vec3(0.9f, 0.f, 0.25f));

    // flat turn
    controlPoints.push_back(vec3(1.f, 0.f, 0.1f));
    controlPoints.push_back(vec3(1.f, 0.f, -0.025));
    controlPoints.push_back(vec3(0.9f, 0.f, -0.15f));

    // run up to loop
    controlPoints.push_back(vec3(0.75f, 0.f, -0.15f));
    controlPoints.push_back(vec3(0.2f, 0.f, -0.15f));
    
    // loop
    controlPoints.push_back(vec3(-0.2f, 0.f, -0.15f));
    controlPoints.push_back(vec3(-0.175f, 0.3f, -0.175f));
    controlPoints.push_back(vec3(0.f, 0.4f, -0.2f));
    controlPoints.push_back(vec3(0.175f, 0.3f, -0.225f));
    controlPoints.push_back(vec3(0.2f, 0.f, -0.25f));

    // run away from loop
    controlPoints.push_back(vec3(-0.2f, 0.f, -0.25f));
    controlPoints.push_back(vec3(-0.75f, 0.f, -0.25f));

    // raised curve and join
    controlPoints.push_back(vec3(-0.9f, 0.f, -0.25f));
    controlPoints.push_back(vec3(-1.f, 0.25f, 0.f));
    controlPoints.push_back(vec3(-0.9f, 0.f, 0.25f));
    controlPoints.push_back(vec3(-0.75f, 0.f, 0.25f));
    */


	bSpline(3, vertices);
}

void generateTrackTangents(vector<vec3> &vertices, vector<vec3> &tangents)
{
    for (int i = 0; (unsigned int)i < vertices.size(); i++)
    {
        tangents.push_back(vertices[i - 1 % vertices.size()] - vertices[i + 1 % vertices.size()]);
    }
}