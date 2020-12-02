#version 410 core

in vec4 vPosition;
in vec4 vColor;
in vec4 vNormal;

out vec3 Normal;
out vec3 FragPos;
out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Color = vColor;
    FragPos = vec3(model * vPosition);
    Normal = mat3(transpose(inverse(model))) * (vNormal.xyz);
    gl_Position = projection * view * model * vPosition;
} 