#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 oPos;

uniform mat4 model;
uniform mat4 viewProj;

void main()
{
    oPos = aPos;
    gl_Position = viewProj * model * vec4(aPos, 1.0);
}