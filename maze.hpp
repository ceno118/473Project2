#ifndef MAZE_HPP
#define MAZE_HPP
#include "basic_shape.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "import_object.hpp"

class Maze {
    protected:
        // std::vector<VAOStruct> vaos;
        // std::vector<Shader> shaders;

        BasicShape walls;
        BasicShape floor;
        BasicShape targets;

        unsigned int walls_texture;
        unsigned int floor_texture;
        unsigned int target_texture;

    public:
        Maze(BasicShape walls, BasicShape floor, BasicShape targets,
        unsigned int walls_texture, unsigned int floor_texture, unsigned int target_texture);

        void CreateMaze(ImportOBJ importer, VAOStruct import_vao);

        void Draw(Shader* shader);
};

#endif //MAZE_HPP