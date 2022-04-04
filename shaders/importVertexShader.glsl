#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoords;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 spec_color;

uniform mat4 projection, view, model, transform;

out vec2 texture_coordinates;
out vec3 normal_vector;
out vec4 fragment_position;
out vec3 color_vec;
out vec3 spec_color_vec;

void main()
{
    texture_coordinates = textCoords;
    normal_vector = mat3(transpose(inverse(model*transform)))*normal;
    fragment_position = model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    color_vec = color;
    spec_color_vec = spec_color;
    gl_Position = projection * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
