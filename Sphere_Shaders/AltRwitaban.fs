#version 410 core

in vec2 frag_Pos;

uniform float earth_radius;
uniform vec4 earth_center;
uniform vec3 light_dir;

out vec4 frag_color;

const int numOutScatter = 10;
const int numInScatter = 10;

vec3 e = vec3(frag_Pos, -1/tan(radians(35.0f)));
float g = -0.85f;
float outerscale = 67.0f/64.0f;
float atm_radius = outerscale * earth_radius;
float H = (outerscale - 1.0f) * earth_radius/4.0f;
float K_R = 0.166;
float K_M = 0.0025;
vec3 C_R = vec3(0.3, 0.7, 1.0);
float PI = radians(180.0f);

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

float Rayleigh(float c)
{
	float cc = c * c;
	return 0.75f * (1 + cc);
}

float height(vec3 p)
{
	return length(p - earth_center.xyz) - earth_radius;
}

float density(vec3 p)
{
	return exp(-height(p)/H);
}

float outScatter(vec3 p, vec3 q)
{
	vec3 step = (q-p) / numOutScatter;
	vec3 v = p + step * 0.5;

	float sum = 0.0;
	for(int i=0; i < numOutScatter; i++)
	{
		sum += density(v);
		v += step;
	}
	sum *= length(step)/(atm_radius-earth_radius);
	return sum;
}

bool intersection_detect(vec3 X0, vec3 L, vec3 R, float r)
{
	float a = dot(L, L);
	float b = 2 * dot((X0 - R), L);
	float c = dot(X0 - R, X0 - R) - r * r;

	return b * b > 4 * a * c && !(b > 0 && c > 0);
}

vec2 intersection_find(vec3 X0, vec3 L, vec3 R, float r)
{
	float a = dot(L, L);
	float b = 2 * dot((X0 - R), L);
	float c = dot(X0 - R, X0 - R) - r * r;

	float D = sqrt(b*b - 4 * a * c);
	float num1 = -b - D;
	float num2 = -b + D;
	float den = 2 * a;

	float near = num1/den;
	float far = num2/den;

	return vec2(near, far);
}

vec3 inScatter(vec3 dir, vec3 pA, vec3 pB, vec3 l)
{
	vec3 step = (pB - pA) / numInScatter;

	vec3 v = pA + step * 0.5;

	vec3 sum = vec3(0.0f);

	for(int i = 0; i < numInScatter; i++)
	{
		vec2 f = intersection_find(v, l, earth_center.xyz, atm_radius);
		if(!intersection_detect(v, l, earth_center.xyz, earth_radius))
		{

			vec3 u = v + l * f.y;

			float n = ( outScatter(pA, v) + outScatter(v, u) ) * PI * 4.0f;

			sum += density(v) * exp(-n * (K_R * C_R + K_M));
		}

		v+= step;
	}
	
	sum *= length(step)/(length(dir) * (atm_radius - earth_radius));
	float c = dot(normalize(dir), normalize(-l));
	return sum * (K_R * C_R * Rayleigh(c) + K_M * Mie(c) ) * 23;
}

void main(void)
{
	if( intersection_detect(vec3(0.0f), e, earth_center.xyz, atm_radius) )
	{
		if( !intersection_detect(vec3(0.0f),e,earth_center.xyz, earth_radius) )
		{
			vec2 f = intersection_find(vec3(0.0f), e, earth_center.xyz, atm_radius);
			vec3 p = f.x * e;
			vec3 q = f.y * e;
			if(f.x < 0)
				p = e;
			vec3 I = inScatter(e, p, q, light_dir);
			float alpha = exp(-outScatter(p, q) * 4 * PI * (K_R+ K_M));
			frag_color = vec4(I,alpha);
		}
		else
		{
			vec2 f = intersection_find(vec3(0.0f), e, earth_center.xyz, atm_radius);
			vec2 g = intersection_find(vec3(0.0f), e, earth_center.xyz, earth_radius);
			vec3 p = f.x * e;
			vec3 q = g.x * e;
			if(f.x < 0)
				p = e;
			vec3 I = inScatter(e, p, q, light_dir);
			float alpha = exp(-outScatter(p, q) * 4 * PI * (K_R + K_M));
			frag_color = vec4(I, alpha);
		}
	}
	else
	{
		discard;
	}
}
