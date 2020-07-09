#version 410 core

//uniform sampler2D s2Tex1;
//uniform sampler2D s2Tex2;

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

in vec3 c0;
in vec3 c1;
in vec3 vNormal;
in vec3 vUv;            // frag pos

uniform vec3 viewPos;
uniform bool blinn;

vec3 ambient;
vec3 diffuse;
vec3 specular;

out vec4 color;

void CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 d, vec3 s, vec3 n)
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
        spec = pow(max(dot(viewDir, reflectDir), 0.0), (8.0));
    }
    
    // combine results
    ambient  = n;
    diffuse  = light.diffuse  * diff * d;
    specular = light.specular * spec * s;
}

void main (void)
{
	vec3 diffuseTex = vec3(texture( tDiffuse, vUv ));
	vec3 diffuseNightTex = vec3(texture( tDiffuseNight, vUv ));
    vec3 specularTex = vec3(texture( tSpecular, vUv ));

    vec3 norm = normalize(vNormal);
    vec3 lightDir = dirLight.direction;
    vec3 viewDir = normalize(viewPos - vUv);

    CalcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex, diffuseNightTex);

    // ambient night
    vec3 night = fNightScale * ambient * (1 - c0);
	vec3 day = diffuse * c0;
    vec3 ocean = specular * c0;

	color = vec4(c1, 1.0) + vec4(night, 1.0);
}