#ifndef BULLET_HPP
#define BULLET_HPP
#include "basic_shape.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "import_object.hpp"
#include "player.hpp"

class Bullet{
    protected:
        std::vector<VAOStruct> vaos;
        std::vector<Shader> shaders;

        //glm::vec3 location;
        

    public:
        Bullet(BasicShape body, Player* player);
        glm::vec3 location;
        float angle_z;
        BasicShape shape;
        BasicShape* shapepointer;
        Player* player;
        void Draw(Shader* shader);
        void Move();
        void Delete();
        void Reset();
        void Shoot(glm::vec3 location, float angle);
        glm::vec3 GetLocation();
        void setMaterial(Material m);
        
};


#endif