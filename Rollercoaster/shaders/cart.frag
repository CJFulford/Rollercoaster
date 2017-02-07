#version 430 core

out vec4 color;

in vec3 vector;
in vec3 boxNorm;

uniform vec3 sunLocation;
uniform vec3 sunColour;

vec3 diffuseColour = vec3(0.7f, 0.7f, 0.75f);
vec3 specularColour = vec3(1.f, 1.f, 1.f);
vec3 ambientColour = vec3(0.2f, 0.2f, 0.2f);

float phongExp = 1000000.f;


void main (void)
{ 
    vec3 H = (sunLocation + vector) / length(sunLocation + vector);

    float diffuse = max(0.f, max(   dot(normalize(sunLocation), normalize(boxNorm)),
                                    dot(normalize(sunLocation), -normalize(boxNorm))
                                )
                        );
    float specular = pow(max(0.f, max(  dot(normalize(H), normalize(boxNorm)),
                                        dot(normalize(H), -normalize(boxNorm))
                                    )
                            ), phongExp);

	color = vec4(
                ambientColour + sunColour * ((diffuseColour * diffuse) + (specularColour * specular))
            , 0.f);
}
