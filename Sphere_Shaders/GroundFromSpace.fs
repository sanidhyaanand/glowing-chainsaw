#version 410 core

struct DirLight{
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform float fNightScale;
uniform samplerCube tDiffuse;
uniform samplerCube tDiffuseNight;
uniform samplerCube tSpecular;
uniform samplerCube tDiffuseClouds;

in vec3 c0;
in vec3 c1;
in vec3 vNormal;
in vec3 vUv;            // frag pos

uniform vec3 viewPos;
uniform bool blinn;

vec3 ambient;
vec3 diffuse;
vec3 specular;
vec3 clouds;

out vec4 color;

void CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 d, vec3 n, vec3 s, vec3 c)
{
    vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0f);
    }
    else
    {
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    
    // combine results
    ambient = (1.0 - diff) * n;
    diffuse  = diff * d;
    specular = light.specular * spec * s;
    clouds = diff * c;
}

void main (void)
{
	vec3 diffuseTex = (texture( tDiffuse, vUv )).rgb;
	vec3 diffuseNightTex = (texture( tDiffuseNight, vUv )).rgb;
    vec3 specularTex = (texture( tSpecular, vUv )).rgb;
    vec3 cloudTex = (texture( tDiffuseClouds, vUv )).rgb;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = dirLight.direction;
    vec3 viewDir = normalize(viewPos - vUv);

    CalcDirLight(dirLight, norm, viewDir, diffuseTex, diffuseNightTex, specularTex, cloudTex);

    // everything
    vec3 night = fNightScale * ambient;
	vec3 day = diffuse * (1 - c0);
    vec3 ocean = specular * 0.5;

    vec3 result = day + night + ocean + clouds; 

	color = vec4(c1, 1.0) + vec4(result, 1.0);
}