#ifndef SPACESHIP_HPP
#define SPACESHIP_HPP

#include "build_shapes.hpp"


class Spaceship {
    protected:
        std::vector<VAOStruct> vaos;
        std::vector<Shader> shaders;
        std::vector<glm::vec4> colors;
        std::vector<BasicShape> shapes;
        float angle_z;
        glm::vec3 velocity;
        glm::vec3 location;
        glm::vec3 scale;
        float last_thrust;

    public:

        Spaceship();


        void CreateShip (std::vector<VAOStruct> vaos, std::vector<Shader> shaders, glm::vec3 location=glm::vec3(0.0),
                         glm::vec3 scale=glm::vec3(0.0), float angle=0.0);

        void Rotate (float angle_change);

        void Thrust (float speed);

        void Move (glm::vec2 bound_x, glm::vec2 bound_y);

        void Draw (std::vector<unsigned int> textures);

        glm::vec3 GetLocation ();

        ~Spaceship();
};

#endif //SPACESHIP_HPP