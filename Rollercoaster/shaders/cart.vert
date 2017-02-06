#version 430 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 tangent;
layout (location = 2) in vec3 normal;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 vert;
out vec3 tang;
out vec3 norm;

void main (void)
{
    vert = vertex;
    tang = tangent;
    norm = normal;
    gl_Position = vec4(vertex, 1.f);
}
