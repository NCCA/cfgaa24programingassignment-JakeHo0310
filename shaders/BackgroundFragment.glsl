#version 410 core

out vec4 FragColor;

uniform vec4 backgroundColor;

void main()
{
    FragColor = backgroundColor;
}