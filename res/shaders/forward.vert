#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 v2fTexCoord;
out vec3 v2fNormal;
out vec3 v2fPos;

uniform mat4 model;
uniform mat4 viewProjection;

uniform mat3 normMat33;


void main()
{
    gl_Position = viewProjection * model * vec4(pos, 1.f);

    v2fTexCoord = texcoord;
    v2fNormal = normMat33 * normal;
    v2fPos = vec3(model * vec4(pos, 1.f));
}
