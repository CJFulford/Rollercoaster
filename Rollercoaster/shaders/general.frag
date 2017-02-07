#version 430 core

const vec3 ambientColour = vec3(0.5f, 0.5f, 0.5f);


const vec3 groundNormal = vec3(0.f, 1.f, 0.f);
const float phongExp = 7;

out vec4 color;

in vec3 vert;

uniform vec3 sunLocation;
uniform vec3 sunColour;
uniform vec3 colourIn;


void main (void)
{
    vec3 H = (sunLocation + vert) / length(sunLocation + vert);

    float diffuse = max(0.f, dot(normalize(sunLocation), groundNormal));
    float specular = pow(max(0.f, dot(normalize(H), groundNormal)), phongExp);

	color = vec4(
                ambientColour + sunColour * colourIn * (diffuse + specular)
            , 0.f);
}
