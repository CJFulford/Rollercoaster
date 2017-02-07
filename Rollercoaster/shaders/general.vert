#version 430 core

layout (location = 0) in vec3 vertex;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 vert;

void main (void)
{
    vert = vertex;
    gl_Position = projection * modelview* vec4(vertex, 1.f);
}
