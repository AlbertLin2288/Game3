#version 330 core
in vec2 gridPos;
out vec4 FragColor;
uniform float cellSize;

void main(){
    vec2 pos = gridPos.xy / cellSize;

    vec2 g = abs(fract(pos - 0.5) - 0.5) / fwidth(pos);
    float line = min(g.x, g.y);
    if(line < 1.0)
        FragColor = vec4(0.8,0.8,0.8,0.3 * (1.0 - line));
    else
        discard;
}