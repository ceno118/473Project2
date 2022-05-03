#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec4 ambient;

uniform bool use_nvg;

void main(){
    if (use_nvg) {
        FragColor = ambient * texture(skybox, TexCoords) * vec4 (0.0, 1.0, 0.0, 1.0);
        return;
    }
    FragColor = ambient * texture(skybox, TexCoords)
}