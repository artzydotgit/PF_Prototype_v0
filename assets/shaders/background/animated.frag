#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 topColor;
uniform vec4 bottomColor;
uniform float time;

void main()
{
    vec2 uv = TexCoord;    
    float offset = sin(time * 0.5) * 0.1;
    uv.y += offset;    
    vec4 color = mix(bottomColor, topColor, uv.y);    
    float wave = sin(uv.x * 10.0 + time) * 0.05;
    color.rgb += wave;
    FragColor = color;
}