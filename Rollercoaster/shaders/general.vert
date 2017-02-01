#version 430 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 tangent;

out vec3 vert;
out vec3 tang;

void main (void)
{
    vert = vertex;
    tang = tangent;
    gl_Position = vec4(vertex, 1.f);
}
