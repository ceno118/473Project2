#include "basic_shape.hpp"

BasicShape::BasicShape () {

}

unsigned int BasicShape::GetVBO () {
    return this->vbo;
}

void BasicShape::Initialize (VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices, int primitive)
{
    this->vao = vao;
    glGenBuffers(1,&(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
    glBufferData(GL_ARRAY_BUFFER,data_byte_size,vertex_data,GL_STATIC_DRAW);
    this->vao_init = true;
    this->vbo_init = true;
    this->vbo_delete = true;
    this->number_vbo_vertices = number_vertices;
    this->primitive = primitive;
}

void BasicShape::InitializeEBO(unsigned int *indices, int indices_bytes, int number_indices, int primitive) {
    glGenBuffers(1,&(this->ebo));
    glBindVertexArray(this->vao.id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices_bytes,indices,GL_STATIC_DRAW);
    this->ebo_init = true;
    this->ebo_delete = true;
    this->ebo_number_indices = number_indices;
    this->ebo_primitive = primitive;
}

void BasicShape::DrawEBO(float line_width) 
{
    BindVAO(this->vao,this->vbo);
    glLineWidth(line_width);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo);
    glDrawElements(this->ebo_primitive,this->ebo_number_indices,
                  GL_UNSIGNED_INT,0);
    glLineWidth(1.0);
}

void BasicShape::SetMaterial(Material m)
{
    this->material = m;
}

void BasicShape::UseMaterial(Shader s)
{
    s.setVec4("material.ambient",this->material.ambient);
    s.setVec4("material.diffuse",this->material.diffuse);
    s.setVec4("material.specular",this->material.specular);
    s.setFloat("material.shininess",this->material.shininess);
    s.setBool("use_material",true);
}

void BasicShape::Draw (Shader shader) 
{
    glUseProgram(shader.ID);
    this->Draw();
    
}

void BasicShape::Draw ()
{
    BindVAO(this->vao,this->vbo);
    glDrawArrays(this->primitive,0,this->number_vbo_vertices);
}

BasicShape::~BasicShape() {

}

void BasicShape::DeallocateShape() {
    if (ebo_delete) {
        glDeleteBuffers(1,&(this->ebo));
    }
    glDeleteBuffers(1,&(this->vbo));
}