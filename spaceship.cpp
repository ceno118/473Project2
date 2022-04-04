#include "spaceship.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Spaceship::Spaceship() {
    this->angle_z =0.0;
    this->location = glm::vec3(0.0);
    this->velocity = glm::vec3(0.0);
}

void Spaceship::CreateShip (std::vector<VAOStruct> vaos,std::vector<Shader> shaders, 
                           glm::vec3 location, glm::vec3 scale, float angle) {
    this->angle_z = angle;
    this->location = location;
    this->scale = scale;
    VAOStruct position_vao = vaos[0];
    VAOStruct texture_vao = vaos[1];
    //thruster,left_canon, right_canon, wing, hull, windshield
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(-0.11,-0.2,0.0),0.22,0.4));
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(0.18,-0.1,0.0),0.05,0.2));
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(-0.23,-0.1,0.0),0.05,0.2));
    this->shapes.push_back(GetTriangle(position_vao,0.25,glm::vec3(0.0,0.15,0.0)));
    this->shapes.push_back(GetHull(texture_vao,glm::vec3(-0.13,-0.1,0.0),0.26,0.4,true));
    this->shapes.push_back(GetHull(position_vao,glm::vec3(-0.1,0.2,0.0),0.2,0.06));
    
    //A vector of colors for the shapes
    this->colors = std::vector<glm::vec4> {
                                   glm::vec4(0.0,1.0,1.0,1.0),
                                   glm::vec4(0.7,0.3,0.3,1.0),
                                   glm::vec4(0.7,0.3,0.3,1.0),
                                   glm::vec4(0.5,0.5,0.6,1.0),
                                   glm::vec4(0.5,0.5,0.6,1.0),
                                   glm::vec4(0.4,0.4,0.9,1.0),
                                   };
    //A vector of shaders for the shapes
    this->shaders = shaders;

}

void Spaceship::Draw(std::vector<unsigned int> textures) {
        glm::mat4 ship_transform = glm::mat4(1.0);
        //add user-induced movement here.
        ship_transform = glm::translate(ship_transform,this->location);
        ship_transform = glm::rotate(ship_transform,
                                     glm::radians(this->angle_z),
                                     glm::vec3(0.0,0.0,1.0));


        //orient and scale the ship before doing anything else.
        ship_transform = glm::scale(ship_transform,glm::vec3(0.2,0.2,1.0));
        ship_transform = glm::rotate(ship_transform,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));\
        ship_transform = glm::translate(ship_transform,glm::vec3(0.0,-0.1,0.0));

        glm::mat4 thruster_transform = glm::mat4(1.0);
        //add user-induced movement here.
        thruster_transform = glm::translate(thruster_transform,this->location);
        thruster_transform = glm::rotate(thruster_transform,
                                     glm::radians(this->angle_z),
                                     glm::vec3(0.0,0.0,1.0));


        //orient and scale the ship before doing anything else.
        glm::vec3 scale_vec {0.0,1.0,1.0};
        float diff = glfwGetTime() - last_thrust;
        if (diff < 0.5) {
            scale_vec.x = (0.5-diff)/0.5;
        }
        thruster_transform = glm::scale(thruster_transform,scale_vec);
        thruster_transform = glm::scale(thruster_transform,glm::vec3(0.2,0.2,1.0));
        thruster_transform = glm::rotate(thruster_transform,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
        thruster_transform = glm::translate(thruster_transform,glm::vec3(0.0,-0.2,0.0));
        

        unsigned int hull_texture = textures[0];

        Shader shader = this->shaders[0];
        Shader texture_shader = this->shaders[1];
        shader.use();
        shader.setMat4("transform",thruster_transform);
        shader.setVec4("set_color",glm::vec4(0.0,1.0,1.0,1.0));
        this->shapes[0].Draw();
        std::vector<Shader*> shader_ptrs = {&shader,&shader,&shader,&shader,&texture_shader,&shader};

        glBindTexture(GL_TEXTURE_2D,hull_texture);
        for (int i = 1; i < shapes.size(); i++) {
            shader_ptrs[i]->use();
            shader_ptrs[i]->setMat4("transform",ship_transform);
            shader_ptrs[i]->setVec4("set_color",colors[i]);
            this->shapes[i].Draw();
            shader_ptrs[i]->setBool("use_color",true);
            shader_ptrs[i]->setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
            this->shapes[i].DrawEBO();
            shader_ptrs[i]->setBool("use_color",false);
        }

}

void Spaceship::Thrust (float speed) {
    this->velocity += glm::vec3(speed*cos(glm::radians(angle_z)),speed*sin(glm::radians(angle_z)),0.0);
    this->last_thrust = glfwGetTime();
}

void Spaceship::Rotate (float angle_change) {
    this->angle_z += angle_change;
}

void Spaceship::Move(glm::vec2 bound_x, glm::vec2 bound_y) {
    if ((this->location.x+this->velocity.x > bound_x[0]) &&
        (this->location.x+this->velocity.x < bound_x[1])) {
            this->location.x += this->velocity.x;
    } else {
        this->velocity.x = 0.0;
    }
    if ((this->location.y+this->velocity.y > bound_y[0]) &&
        (this->location.y+this->velocity.y < bound_y[1])) {
            this->location.y += this->velocity.y;
    } else {
        this->velocity.y = 0.0;
    }
}

glm::vec3 Spaceship::GetLocation() {
    return this->location;
}

Spaceship::~Spaceship () {
    for (int i = 0; i < this->shapes.size(); i++) {
        unsigned int vbo_id = this->shapes[i].GetVBO();
        glDeleteBuffers(1,&vbo_id);
    }
}