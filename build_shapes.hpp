#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP
#include "basic_shape.hpp"

//Creates and returns a BasicShape triangle given a VAO, a float size, and a location (centered on that location)
BasicShape GetTriangle (VAOStruct vao,float size=1.0, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a textured BasicShape triangle given a VAO and three points (glm::vec3 type)
BasicShape GetTexturedTriangle (VAOStruct vao, glm::vec3 vertex_one, glm::vec3 vertex_two, glm::vec3 vertex_three);

//Creates and returns a BasicShape circle given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, bool texture = false, int points= 40, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape rectangle given a VAO, a point to start from, a width (x) and height (y).
BasicShape GetRectangle (VAOStruct vao, glm::vec3 location = glm::vec3(0.0), float width = 1.0, float height = 1.0);

//Creates and returns a BasicShape rectangle with texture data given a VAO, a point to start from, a width (x) and height (y).
BasicShape GetTexturedRectangle (VAOStruct vao, glm::vec3 location = glm::vec3(0.0), 
                                 float width = 1.0, float height = 1.0, 
                                 float texture_size=1.0);

//Creates a ship hull placed at the provided lower_left point, width, and height.
BasicShape GetHull (VAOStruct vao, glm::vec3 lower_left = glm::vec3(0.0),float width=1.0,float height=1.0,bool use_texture = false);

//Create a rudimentary starfield given a number of stars.
BasicShape GetStars (VAOStruct vao, int num_stars = 1);

//Generates a texture and returns it (default wrapping and filtering assumed).
unsigned int GetTexture (std::string file_name,bool flip_on_load=false);

#endif //BUILD_SHAPES_HPP