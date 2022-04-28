#include "maze.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Maze::Maze(BasicShape walls, BasicShape floor, BasicShape targets,
            unsigned int walls_texture, unsigned int floor_texture, unsigned int target_texture){
    this -> walls = walls;
    this -> floor = floor;
    this -> targets = targets;
};

void Maze::Draw(Shader* shader){
    shader -> use();
    shader -> setMat4("transform", glm::mat4(1.0));
    glm::mat4 walls_model = glm::mat4(1.0);
    walls_model = glm::rotate(walls_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
    walls_model = glm::translate(walls_model, glm::vec3(0.0));
    walls_model = glm::scale(walls_model, glm::vec3(1.5));
    shader -> setMat4("model", walls_model);
    shader -> setBool("use_texture", true);
    glBindTexture(GL_TEXTURE_2D, walls_texture);
    shader -> setMat4("transform", glm::mat4(1.0f));
    walls.Draw();

    shader -> setMat4("transform", glm::mat4(1.0));
    glm::mat4 floor_model = glm::mat4(1.0);
    floor_model = glm::rotate(floor_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
    floor_model = glm::translate(floor_model, glm::vec3(0.0));
    floor_model = glm::scale(floor_model, glm::vec3(1.5));
    shader -> setMat4("model", floor_model);
    shader -> setBool("use_texture", true);
    glBindTexture(GL_TEXTURE_2D, floor_texture);
    shader -> setMat4("transform", glm::mat4(1.0f));
    floor.Draw();

    shader -> setMat4("transform", glm::mat4(1.0));
    glm::mat4 targets_model = glm::mat4(1.0);
    targets_model = glm::rotate(targets_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
    targets_model = glm::translate(targets_model, glm::vec3(0.0));
    targets_model = glm::scale(targets_model, glm::vec3(1.5));
    shader -> setMat4("model", targets_model);
    shader -> setBool("use_texture", true);
    glBindTexture(GL_TEXTURE_2D, target_texture);
    shader -> setMat4("transform", glm::mat4(1.0f));
    targets.Draw();
}
    