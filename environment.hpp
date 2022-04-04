#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>

#include <iostream>
#include <vector>
#include <string>


using namespace std;

//Initialize the environment by initializing GLFW and GLAD and creating 
// the window and returning a pointer to it.
GLFWwindow* InitializeEnvironment (string window_title, int window_width, int window_height);

//Resizes the window.  Invoked when the user attempts to resize the window.
//Receives a GLFWwindow pointer, width integer, and height integer as input.
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 

//Displays the values of a color defined by a glm::vec4 of float values.
void PrintColor (glm::vec4 vec);

#endif //ENVIRONMENT_HPP