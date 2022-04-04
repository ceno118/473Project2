#include "vertex_attribute.hpp"

AttributePointer BuildAttribute (int vals_per_vertex, int data_type,bool normalized, int stride_bytes, int offset_bytes) 
{
    AttributePointer attribute;
    attribute.values_per_vertex = vals_per_vertex;
    attribute.type_of_data = data_type;
    attribute.normalized = normalized;
    attribute.stride_bytes = stride_bytes;
    attribute.offset_bytes = offset_bytes;
    return attribute;

}

void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type) 
{
    glBindVertexArray(vao.id);
    glBindBuffer(buffer_type,vbo);
    for (int i=0; i < vao.attributes.size(); i++) {
        AttributePointer attr = vao.attributes[i];
        glVertexAttribPointer(i,attr.values_per_vertex,attr.type_of_data,
                             attr.normalized,attr.stride_bytes,(void*)(intptr_t)attr.offset_bytes);
        glEnableVertexAttribArray(i);
    }

}