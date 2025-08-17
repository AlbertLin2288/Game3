#version 330 core
in vec2 gridPos;
in vec3 worldPos;
out vec4 FragColor;
uniform float cellSize;
uniform vec2 offset;

void main(){
    if (length(worldPos) > 1) discard;

    vec2 pos;
    pos.y = asin(gridPos.y / sqrt(1-gridPos.x*gridPos.x));
    pos.x = asin(gridPos.x / sqrt(1-gridPos.y*gridPos.y));
    pos = (pos + offset) / cellSize;

    vec2 g = abs(fract(pos - 0.5) - 0.5) / fwidth(pos);
    float line = min(g.x, g.y);
    if(line < 1.0)
        FragColor = vec4(0.8,0.8,0.8,0.3 * (1.0 - line));
    else
        discard;
}