#version 330 core
in vec3 oPos;
out vec4 FragColor;

uniform vec4 ourColor;

void main() {
    if (length(oPos)>1.4142135623730)discard;
    FragColor = ourColor;
}