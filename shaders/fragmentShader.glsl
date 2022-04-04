#version 330 core

out vec4 FragColor;


uniform vec4 set_color;


void main()
{
    FragColor = set_color;
}