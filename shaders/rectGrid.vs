#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 invViewProj;

out vec3 worldPos;

void main() {
    vec4 worldPos4 = invViewProj * vec4(aPos, 0.999, 1.0);
    worldPos = worldPos4.xyz / worldPos4.w;
    gl_Position = vec4(aPos, 0.0, 1.0);
}