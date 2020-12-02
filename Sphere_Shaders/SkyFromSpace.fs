#version 410 core

in vec3 c0;
in vec3 c1;
in vec3 v3Direction;

uniform float g;
uniform float g2;
uniform vec3 v3LightPos;

out vec4 color;

float Rayleigh(float c)
{
	float cc = c * c;
	return 0.75f * (1 + cc);
}

float Mie(float c)
{
	float cc = c * c;
	float gg = g * g;

	float a = (1 - gg) * (1 + cc);
	float b = 1 + gg - 2 * g * c;
	b *= sqrt(b);
	b *= 2 + gg;

	return 1.5f * a/b;
}

void main()  
{   
	vec3 lightPos = normalize(v3LightPos);
	float fCos = dot(lightPos, v3Direction) / length(v3Direction);   
	//vec3 result = Rayleigh(fCos) * c0 + Mie(fCos) * c1;
	vec3 expPart = 2 * Rayleigh(fCos) * c0 + Mie(fCos) * c1;
	vec3 result = 1.0 - exp(-expPart);
	color = vec4(result, 1.0);
	color.a = color.b;
} 

