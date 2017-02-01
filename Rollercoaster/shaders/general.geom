#version 430 core

layout(lines) in;
layout(triangle_Strip, max_vertices = 6) out;


uniform mat4 modelview;
uniform mat4 projection;

in vec3 vert[];

const float PI = 3.14159265359;



//Rodrigues' rotation formula
vec3 rotAny (vec3 vector, vec3 axis, float angle) 
{
	return vec3((vector * cos(angle)) + 
				(cross(axis, vector) * sin(angle)) + 
				(axis * dot(axis, vector) * (1.f - cos(angle))));
}
mat3 rotateX(float a){return mat3(1.f, 0.f, 0.f, 0.f, cos(a), -sin(a), 0.f, sin(a), cos(a));}
mat3 rotateY(float a){return mat3(cos(a), 0.f, sin(a), 0.f, 1.f, 0.f, -sin(a), 0.f, cos(a));}
mat3 rotateZ(float a){return mat3(cos(a), -sin(a), 0.f, sin(a), cos(a), 0.f, 0.f, 0.f, 1.0);}

float width = 0.01f;

void main (void)
{	
    gl_Position = projection * modelview * vec4(vert[0] + vec3(0.f, 0.f, width), 1.f);
    EmitVertex();

    gl_Position = projection * modelview * vec4(vert[0] + vec3(0.f, 0.f, -width), 1.f);
    EmitVertex();

    gl_Position = projection * modelview * vec4(vert[1] + vec3(0.f, 0.f, width), 1.f);
    EmitVertex();

    EndPrimitive();


    gl_Position = projection * modelview * vec4(vert[0] + vec3(0.f, 0.f, -width), 1.f);
    EmitVertex();

    gl_Position = projection * modelview * vec4(vert[1] + vec3(0.f, 0.f, -width), 1.f);
    EmitVertex();

    gl_Position = projection * modelview * vec4(vert[1] + vec3(0.f, 0.f, width), 1.f);
    EmitVertex();

    EndPrimitive();
}












