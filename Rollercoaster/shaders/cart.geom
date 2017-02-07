#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;


uniform mat4 modelview;
uniform mat4 projection;

in vec3 vert[];
in vec3 tang[];
in vec3 norm[];

out vec3 vector;
out vec3 boxNorm;

const float PI = 3.14159265359,
            twoPI = 3.14159265359,
            halfPI = 3.14159265359,
            cartHeightScale = 0.3f,
            cartLengthScale = 0.2f,
            trackWidth = 0.15f;



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

vec4 toOutput(vec3 v)
{
    return projection * modelview * vec4(v, 1.f);
}

void pointToBox(vec3 v, vec3 t, vec3 n, float cartHeight, float cartLength)
{
    
    vec3 vertex = v;
    vec3 tangent = cartLength * normalize(t);
    vec3 perpendicular = trackWidth * normalize(cross(tangent, normalize(n)));
    vec3 normal = cartHeight * normalize(cross(perpendicular, tangent));
    
    // ============================================ outer wall
    vec3 p0 = vertex + perpendicular - tangent;
    vec3 p1 = vertex + perpendicular + tangent;
    boxNorm = normalize(cross(p0, p1));

    gl_Position = toOutput(p0);
    EmitVertex();
    gl_Position = toOutput(p1);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular - tangent + normal);
    EmitVertex();
    EndPrimitive();

    gl_Position = toOutput(vertex + perpendicular + tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular - tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular + tangent);
    EmitVertex();
    EndPrimitive();
    
    // ============================================ inner wall
    p0 = vertex - perpendicular + tangent;
    p1 = vertex - perpendicular - tangent;
    boxNorm = normalize(cross(p0, p1));

    gl_Position = toOutput(p0);
    EmitVertex();
    gl_Position = toOutput(p1);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular - tangent + normal);
    EmitVertex();
    EndPrimitive();

    gl_Position = toOutput(vertex - perpendicular + tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular - tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular + tangent);
    EmitVertex();
    EndPrimitive();
    
    // ============================================ front wall
    p0 = vertex - perpendicular + tangent;
    p1 = vertex + perpendicular + tangent;
    boxNorm = normalize(cross(p0, p1));

    gl_Position = toOutput(p0);
    EmitVertex();
    gl_Position = toOutput(p1);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular + tangent + normal);
    EmitVertex();
    EndPrimitive();

    gl_Position = toOutput(vertex - perpendicular + tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular + tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular + tangent);
    EmitVertex();
    EndPrimitive();
    
    // ============================================ back wall
    p0 = vertex - perpendicular - tangent;
    p1 = vertex + perpendicular - tangent;
    boxNorm = normalize(cross(p0, p1));

    gl_Position = toOutput(p0);
    EmitVertex();
    gl_Position = toOutput(p1);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular - tangent + normal);
    EmitVertex();
    EndPrimitive();
    
    gl_Position = toOutput(vertex - perpendicular - tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular - tangent + normal);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular - tangent);
    EmitVertex();
    EndPrimitive();
    
    // ============================================ floor
    p0 = vertex - perpendicular - tangent;
    p1 = vertex - perpendicular + tangent;
    boxNorm = normalize(cross(p0, p1));

    gl_Position = toOutput(p0);
    EmitVertex();
    gl_Position = toOutput(p1);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular - tangent);
    EmitVertex();
    EndPrimitive();

    gl_Position = toOutput(vertex + perpendicular - tangent);
    EmitVertex();
    gl_Position = toOutput(vertex + perpendicular + tangent);
    EmitVertex();
    gl_Position = toOutput(vertex - perpendicular + tangent);
    EmitVertex();
    EndPrimitive();
}

void main (void)
{
    vector = vert[0];
    pointToBox(vert[0], tang[0], norm[0], cartHeightScale, cartLengthScale);
}













