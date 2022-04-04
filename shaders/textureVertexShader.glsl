#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoords;
layout (location = 2) in vec3 normal;

uniform mat4 projection, view, model, transform;

out vec2 texture_coordinates;
out vec3 normal_vector;
out vec4 fragment_position;

void main()
{
    texture_coordinates = textCoords;
    normal_vector = mat3(transpose(inverse(model*transform)))*normal;
    fragment_position = model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = projection * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
