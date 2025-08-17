#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 gridPos;
uniform vec3 dirx;
uniform vec3 diry;
uniform vec2 offset;

uniform mat4 invViewProj;

void main() {
    vec4 worldPos = invViewProj * vec4(aPos.xy, 1.0, 1.0);
    worldPos.xyz /= worldPos.w;
    gridPos.x = dot(worldPos.xyz, dirx);
    gridPos.y = dot(worldPos.xyz, diry);
    gridPos += offset;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}