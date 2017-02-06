#include "Header.h"
#include <fstream>
#include <sstream>
#include <omp.h>


#define trackOrder 6 // silky smooth
#define arcLength 0.075f

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

    for (int i = 0; i < minNumOfKnots; i++)
    {
        if (i < order)
            knots.push_back(0.f);
        else if ((unsigned int)i > controlPoints.size())
            knots.push_back(1.f);
        else
            knots.push_back(knots[i - 1] + (1.f / (controlPoints.size() - order + 1)));
    }

    for (float u = 0.f; u < 1.f; u += 0.000005f)
    {
        vec3 point = vec3(0.f, 0.f, 0.f);
        for (int i = 0; (unsigned int)i < controlPoints.size(); i++)
            point += (controlPoints[i] * deBoor_Cox(i, order, u));

        if (u == 0.f || distance(output.back(), point) > arcLength)
        {
            output.push_back(point);
            maxHeight = max(maxHeight, point.y);
        }
    }

    
    return maxHeight;
}

float generateTrackCurve(vector<vec3> &vertices)
{
    vertices.clear();
    controlPoints.clear();
    knots.clear();
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
    tangents.clear();
    for (int i = 0; (unsigned int)i < vertices.size(); i++)
            tangents.push_back(vertices[(i + 1) % vertices.size()] - vertices[i]);
}

float calculateSpeed(float height, float &totalEnergy)
{
    // total energy - eP = eK
    float eK = max(minKineticEnergy, totalEnergy - (cartMass * gravity * height));
    totalEnergy = eK + (cartMass * gravity * height);
    return sqrt(eK * 2.f / cartMass); // convert eK into velocity
}

void generateTrackNormals(vector<vec3> &vertices, vector<vec3> &tangents, vector<vec3> &normals, float maxHeight)
{
    normals.clear();


    float totalEnergy = minKineticEnergy;   // initial eK + max eP

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        vec3 perpendicular = cross(tangents[i], tangents[(i + 1) % tangents.size()]);


        //what this results in is the direction vector for the centripital acceleration
         vec3 normal = normalize(cross(perpendicular, tangents[i]));

        // this is here so that we dont get the normal flip at the top of the chain lift
        if (i < vertices.size() / 8 && normal.y < 0.f) 
            normal = normalize(-1.f * normal);

        // force of gravity in the applied direction
        vec3 gravDir = vec3(0.f, -gravity, 0.f);

        // find the tangential speed of the cart
        float timeInArc = arcLength / calculateSpeed(vertices[i].y, totalEnergy);

        normal /= timeInArc;

        normals.push_back(normalize(normal - gravDir));

        totalEnergy -= energyLoss;
    }
    
}