#ifndef SHADER_H_
#define SHADER_H_

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//Class is similar to the one defined on www.learnopengl.com.  Primary
// differences include: 1) separate header and source files; 2) use of the
// GLM library to easily set vectors of size 4 (1-3 could be added separately).
class Shader {
public:
    //The ID of the resulting shader program
    unsigned int ID;

    //Constructor for the shader program (takes the path to the
    //vertex and fragment shader GLSL files).
    Shader(const char* vertexPath, const char* fragmentPath);

    //Use the shader program
    void use();

    //Given the name of a uniform (std::string) and a boolean value,
    // sets the uniform to the provided value.
    void setBool(const std::string &name, bool value) const;
    //Given the name of a uniform (std::string) and an integer value,
    // sets the uniform to the provided value.
    void setInt (const std::string &name, int value) const;
    //Given the name of a uniform (std::string) and a float value,
    // sets the uniform to the provided value.
    void setFloat (const std::string &name, float value) const;
    //Given the name of a uniform (std::string) and a vector with four float values,
    // sets the uniform to the vector.
    void setVec4 (const std::string &name, glm::vec4 v) const;

    //Given the name of a uniform (std::string) and a 4x4 float matrix,
    // sets the uniform to the matrix.
    void setMat4 (const std::string &name, glm::mat4 m) const;

private:
    //Internal function used to check for errors during shader compilation.
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif // SHADER_H_
