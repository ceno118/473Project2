#version 330 core

out vec4 FragColor;

in vec2 texture_coordinates;
in vec4 fragment_position;
in vec3 normal_vector;

uniform sampler2D texture1;
uniform vec4 transparentColor;

uniform float alpha;

void main()
{
   vec4 tempColor = texture(texture1,texture_coordinates);
   if (tempColor == transparentColor) {
       tempColor = vec4(1.0,0.0,0.0,alpha);
   }

   FragColor = tempColor;
}
