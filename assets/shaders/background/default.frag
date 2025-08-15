#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;
uniform vec4 topColor;
uniform vec4 bottomColor;
uniform float time;

void main()
{
    FragColor = color;
}
