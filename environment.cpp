#include "environment.hpp"

GLFWwindow* InitializeEnvironment (string window_title, int window_width, int window_height) 
{
    //initialize the GLFW library (MUST be done before GLFW functions are used!)
    glfwInit();

    //GLFW context options in option, value format -- here we set it to Core profile and
    //  OpenGL Version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    //This next line is necessary for Mac OS X.  
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    

    //Create the window object
    GLFWwindow *window = glfwCreateWindow(window_width,window_height,window_title.c_str(),NULL,NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return NULL;
    }

    //makes the context of the specified window current
    glfwMakeContextCurrent(window);

    //Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }  
    

    //Set the size of the rendering window so OpenGL knows how we want to display the coordinates
    //first two parameters set the lower left corner of the window
    //then width then height of the window in pixels
    //Below this means a point at (-0.5, 0.5) in normalized device coordinates ends up at 
    //   location (200, 450)
    glViewport(0,0,window_width,window_height);

    //Set the window resize callback.
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    return window;

}

//The window, width, and height are provided to this function when the window is resized.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void PrintColor (glm::vec4 vec) {
    cout << "Color("<<vec.r<<", "<<vec.g<<", ";
    cout << vec.b << ")" << endl;
}