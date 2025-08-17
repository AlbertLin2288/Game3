#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 gridPos;
out vec3 worldPos;
uniform vec3 dirx;
uniform vec3 diry;

uniform mat4 invViewProj;

void main() {
    vec4 worldPos4 = invViewProj * vec4(aPos.xy, 1.0, 1.0);
    worldPos = worldPos4.xyz /= worldPos4.w;
    gridPos.x = dot(worldPos, dirx);
    gridPos.y = dot(worldPos, diry);
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}