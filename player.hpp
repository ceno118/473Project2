#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "basic_shape.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "import_object.hpp"



class Player {
    protected:
        std::vector<VAOStruct> vaos;
        std::vector<Shader> shaders;
        
        glm::vec3 bullet_loc;
        glm::vec3 bullet_vel;
        BasicShape shape;

    public:
        Player(BasicShape body);
        glm::vec3 location;
        float angle_z;

        void CreatePlayer(ImportOBJ importer, VAOStruct import_vao);

        void Rotate(float angle_change);

        void Move(int dir);
        
        void Draw(Shader shader, bool nvg);

        void process_input(GLFWwindow* window, bool top_cam);
        void setMaterial(Material m);
        glm::vec3 getLocation();


};
#endif