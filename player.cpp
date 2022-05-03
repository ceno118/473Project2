#include "player.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Player::Player(BasicShape body) {
    this -> angle_z = 90.0;
    this -> location = glm::vec3(0.0, -1.2, 10.0);
    this -> bullet_loc = this -> location;
    this -> shape = body;
}

void CreatePlayer(ImportOBJ importer, VAOStruct import_vao){
    importer.loadFiles("./models/low_poly", import_vao);
}

void Player::Rotate(float angle_change){
    this -> angle_z += angle_change;
}

void Player::Move(){
    this -> location.x += 0.002*(cos(glm::radians(-1*(this -> angle_z))));
    this -> location.z += 0.002*(sin(glm::radians(-1*(this -> angle_z))));
}

void Player::Draw(Shader shader, bool nvg){
    shader.use();
    shader.setBool("use_texture", false);
    shader.setMat4("transform", glm::mat4(1.0));
    glm::mat4 player_model = glm::mat4(1.0);

    player_model = glm::translate(player_model, this -> location);
    player_model = glm::rotate(player_model, glm::radians(this -> angle_z -90), glm::vec3(0.0, 1.0, 0.0));
    player_model = glm::scale(player_model, glm::vec3(0.15));
    shader.setMat4("model", player_model);
    shader.setMat4("transform", glm::mat4(1.0));
    if (nvg) {
        shader.setVec4("direction_light.ambient", glm::vec4(0.0, 1.0, 0.0, 1.0));
    }
    else {
        shader . setVec4("direction_light.ambient", glm::vec4(0.3, 0.3, 0.3, 1.0));
    }
    this->shape.Draw();

}

void Player::process_input(GLFWwindow* window, bool top_cam){
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !top_cam){
        this -> Move();
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        this -> Rotate(0.1);
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        this -> Rotate(-0.1);
    }
    
}

void Player::setMaterial(Material m){
    this -> shape.SetMaterial(m);
}

glm::vec3 Player::getLocation(){
    return this -> location;
}