#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>

#include <iostream>
#include <vector>

struct AttributePointer {
    int values_per_vertex;
    int type_of_data;
    bool normalized;
    int stride_bytes;
    int offset_bytes;
};

struct VAOStruct {
    unsigned int id;
    std::vector<AttributePointer> attributes;
};

AttributePointer BuildAttribute (int vals_per_vertex, int data_type,bool normalized, int stride_bytes, int offset_bytes);

void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type = GL_ARRAY_BUFFER);

#endif //ENVIRONMENT_SETUP_HPP 