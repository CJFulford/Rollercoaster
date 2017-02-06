#version 430 core

out vec4 color;

in vec3 vector;
in vec3 boxNorm;

vec3 sun = vec3(5.f, 20.f, 0.f);
vec3 sunColour = vec3(1.f, 1.f, 1.f);

float phongExp = 100.f;


void main (void)
{ 

    vec3 H = (sun + vector) / length(sun + vector);

    float diffuse = max(0.f, max(   dot(normalize(sun), normalize(boxNorm)),
                                    dot(normalize(sun), -normalize(boxNorm))
                                )
                        );
    float specular = pow(max(0.f, max(  dot(normalize(H), normalize(boxNorm)),
                                        dot(normalize(H), -normalize(boxNorm))
                                    )
                            ), phongExp);

	color = vec4(
                vec3(0.f, 1.f, 0.f) * sunColour * diffuse
            , 0.f);
}
