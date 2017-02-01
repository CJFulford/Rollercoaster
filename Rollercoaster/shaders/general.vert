#version 430 core

layout (location = 0) in vec3 vertex;

out vec3 vert;

void main (void)
{
    vert = vertex;
    gl_Position = vec4(vertex, 1.f);
}
