#include "player.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "bullet.hpp"

Bullet::Bullet(BasicShape body, Player* player){
    this -> location = player -> location + glm::vec3(0,0.3,0);
    this -> angle_z = player -> angle_z;
    this -> shape = body;
    this -> shapepointer = &shape;
    this -> player = player;
}

void Bullet::Draw(Shader* shader){
    shader->use();
    shader->setMat4("transform", glm::mat4(1.0));
    glm::mat4 bullet_model = glm::mat4(1.0);

    bullet_model = glm::translate(bullet_model, glm::vec3(this -> location.x, this->location.y + 0.8, this->location.z));
    bullet_model = glm::rotate(bullet_model, glm::radians(this -> angle_z -90), glm::vec3(0.0, 1.0, 0.0));
    bullet_model = glm::scale(bullet_model, glm::vec3(0.2));
    shader->setMat4("model", bullet_model);
    shader->setMat4("transform", glm::mat4(1.0));
    this->shape.Draw();
}

void Bullet::Move(){
    this -> location.x += 0.005*(cos(glm::radians(-1*(this -> angle_z))));
    this -> location.z += 0.005*(sin(glm::radians(-1*(this -> angle_z))));
}

void Bullet::Reset(){
    this->location = player -> location;
    this -> angle_z = player ->angle_z;
}

void Bullet::Delete(){
    this->shape.DeallocateShape();
}

void Bullet::Shoot(glm::vec3 location, float angle){
    this -> location = location;
    this -> angle_z = angle;
}
glm::vec3 Bullet::GetLocation(){
    return this->location;
}

void Bullet::setMaterial(Material m){
    this -> shape.SetMaterial(m);
}