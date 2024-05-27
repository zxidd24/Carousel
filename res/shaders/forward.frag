#version 330 core

in vec2 v2fTexCoord;
in vec3 v2fNormal;
in vec3 v2fPos;

out vec4 fragColor;

uniform vec3 camPos;

uniform vec3 diffuseColor;

void main()
{
    const float ambientStrength = 1.0;

    vec3 normal = normalize(v2fNormal);

    vec3 viewDir = normalize(v2fPos - camPos);
    vec3 finalColor = diffuseColor * ambientStrength;

    fragColor = vec4(finalColor, 1.0);
}
