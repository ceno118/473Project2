#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "camera.hpp"
#include "Font.hpp"
#include "import_object.hpp"
#include "player.hpp"
#include "bullet.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window, Player* player, VAOStruct importVAO, ImportOBJ importer, Bullet bullet);

//Global Variables

bool main_cam = true;
bool top_cam = false;
bool third_cam = false;
bool shot_out = false;
bool red = false;
float rotation_x = 0.0;
float rotation_z = 90.0;

Camera  camera(glm::vec3(2.0,3.0,7.0),glm::vec3(0.0,1.0,0.0),0.0f,-25.0f);
Camera  *curr_camera = &camera;


bool first_mouse = true;
int screen_width = 600;
int screen_height = 600;
double lastX = screen_width/2.0;
double lastY = screen_height/2.0;


void mouse_callback(GLFWwindow* window, double xpos, double ypos);

glm::mat4 view = glm::mat4(1.0);
int main () {
    
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",screen_width,screen_height);
    if (window == NULL) {
        return -1;
    }
    
    
    float ambient_strength = 0.3;
    glm::vec4 light_color = glm::vec4(1.0);
    glm::vec4 light_position = glm::vec4(0.0, 10.0, 0.0, 1.0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback);  

    //Blending....dealing with the alpha channel for transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int floor_texture = GetTexture("./images/hull_texture.png");

    glm::vec4 dir_light_color = glm::vec4(1.0,1.0,1.0,1.0);
    glm::vec4 dir_light_direction = glm::vec4(1.0,-1.0,0.0,0.0);
    
    Shader import_shader("./shaders/importVertexShader.glsl","./shaders/importFragmentShader.glsl");
    Shader texture_shader("./shaders/textureVertexShader.glsl","./shaders/textureFragmentShader.glsl");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    //Vertex Array Object for textured VBOs
    //Each vertex has a position (x,y,z), texture coordinate (s, t), and 
    // a surface normal (sx, sy, sz)
    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer texture_pos_attr = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,8*sizeof(float),3*sizeof(float));
    AttributePointer normal_attr = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),5*sizeof(float));
    texture_vao.attributes.push_back(texture_pos_attr);
    texture_vao.attributes.push_back(texture_attr);
    texture_vao.attributes.push_back(normal_attr);

    //Vertex Array Object for imported VBOs
    VAOStruct import_vao;
    //Each vertex has a position (x,y,z), surface normal (sx,sy,sz)
    // texture coordinate (s, t), color attribute (r,g,b), 
    // and a specular color (r,g,b)
    
    glGenVertexArrays(1,&(import_vao.id));
    AttributePointer import_pos_attr = BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),0);
    AttributePointer import_norm_attr = BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),3*sizeof(float));
    AttributePointer import_tex_attr = BuildAttribute(2,GL_FLOAT,false,14*sizeof(float),6*sizeof(float));
    AttributePointer import_color_attr = BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),8*sizeof(float));
    AttributePointer import_spec_col_attr = BuildAttribute(3,GL_FLOAT,false,14*sizeof(float),11*sizeof(float));
    import_vao.attributes.push_back(import_pos_attr);
    import_vao.attributes.push_back(import_norm_attr);
    import_vao.attributes.push_back(import_tex_attr);
    import_vao.attributes.push_back(import_color_attr);
    import_vao.attributes.push_back(import_spec_col_attr);
    
    
    //Create an import object to import different blender-generated shapes
    
    ImportOBJ importer;
    BasicShape maze = importer.loadFiles("./models/finalmaze",import_vao);
    BasicShape* mazepointer = &maze;
    Player player(importer.loadFiles("./models/low_poly", import_vao));
    Player* player_pointer = &player;
    Bullet bullet(importer.loadFiles("./models/bullet", import_vao), &player);
    Bullet* bullet_pointer = &bullet;

    //Model Location, Scale, and Textures:

    //Set up initial transformations.
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
    
    view = glm::translate(view,glm::vec3(0.0,0.0,-5.0));
    glm::mat4 project = glm::mat4(1.0);
    project = glm::perspective(glm::radians(45.0f),(1.0f*screen_width)/(1.0f*screen_height),0.1f,100.0f);


    std::vector<Shader*> shaders {&texture_shader,&import_shader};
    for (int i = 0; i < shaders.size(); i++)
    {
        shaders[i]->use();
        shaders[i]->setMat4("transform",glm::mat4(1.0));
        shaders[i]->setMat4("model",model);
        shaders[i]->setMat4("view",view);
        shaders[i]->setMat4("projection",project);
        shaders[i]->setBool("point_light.on",false);

        // ambient light
        
        shaders[i]->setVec4("direction_light.direction",dir_light_direction);
        shaders[i]->setVec4("direction_light.ambient",0.3f*dir_light_color);
        shaders[i]->setVec4("direction_light.diffuse",dir_light_color);
        shaders[i]->setVec4("direction_light.specular",dir_light_color);
        shaders[i]->setBool("direction_light.on",true);
        shaders[i]->setVec4("spot_light.position", glm::vec4(player.getLocation().x, player.getLocation().y + 0.8, player.getLocation().z, 1.0));
        shaders[i]->setBool("spot_light.on",false);
    }

    glEnable(GL_DEPTH_TEST);

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        // updates player position and angle based on input
        player.process_input(window, top_cam);
        
        // I was trying to implement a spotlight that followed the player
        // and looked where it was looking, but I couldn't get it to work.

        // import_shader.setVec4("spot_light.position", glm::vec4(player.getLocation().x, player.getLocation().y + 0.5, player.getLocation().z, 1.0));
        // import_shader.setVec4("spot_light.direction", glm::vec4((cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 0.8, (sin(glm::radians(-1*(player_pointer->angle_z)))), 1.0));
        // import_shader.setVec4("spot_light.ambient", glm::vec4(1.0));
        // import_shader.setVec4("spot_light.diffuse", glm::vec4(1.0));
        // import_shader.setVec4("spot_light.specular", glm::vec4(1.0));
        // import_shader.setFloat("spot_light.cutoff", glm::cos(glm::radians(10.0f)));
        // import_shader.setFloat("spot_light.outer_cutoff", glm::cos(glm::radians(10.0f)));
        // import_shader.setBool("spot_light.on", true);

        
        //input
        processInput(window, &player, import_vao, importer, bullet);

        if (red){
           for (int i = 0; i < shaders.size(); i++){
               shaders[i]->setVec4("direction_light.ambient", 0.3f*glm::vec4(1, 0, 0, 1)); 
           }
        }
        else{
            for (int i = 0; i < shaders.size(); i++){
                shaders[i]->setVec4("direction_light.ambient",0.3f*dir_light_color);
            }
        }
        
        // checks if the bullet should be reset
        if (!shot_out){
            bullet.Reset();
        }
        glm::vec3 bullet_loc = bullet.GetLocation();
        if (bullet_loc.x > 10 || bullet_loc.z > 10 || bullet_loc.x < -10 || bullet_loc.z < -10){
            bullet.Reset();
            shot_out = false;
        }      

        //set the clear color to  wipe the window
        glClearColor(0.0,0.0,0.0,1.0);

        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Sets camera type/position based on user input
        if (main_cam){// free flying
            view = curr_camera->GetViewMatrix();
        }
        else if (top_cam){// top down on player
            view = glm::lookAt((player_pointer->location + glm::vec3(0,20,0)), player_pointer->location, glm::vec3(0,0,-1));
        }
        else if (third_cam){// third person
            glm::vec3 lookFrom = glm::vec3(-1.5*(cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 0.8, -1.5*(sin(glm::radians(-1*(player_pointer->angle_z)))));
            glm::vec3 lookTo = glm::vec3((cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 0.8, (sin(glm::radians(-1*(player_pointer->angle_z)))));
            view = glm::lookAt(player_pointer->location + lookFrom, player_pointer->location + lookTo, glm::vec3(0,1,0));
        }
        else {// catches any error where none of the camera bools are set
            main_cam = true;
        }
        
        for (int i = 0; i < shaders.size(); i++) {
            shaders[i]->use();
            shaders[i]->setMat4("view",view);
            shaders[i]->setVec4("eye_position",glm::vec4(camera.Position,1.0));
        }

        texture_shader.use();

        //Draws the maze
        import_shader.use();
        glm::mat4 maze_model = glm::mat4(1.0);
        maze_model = glm::rotate(maze_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
        maze_model = glm::translate(maze_model, glm::vec3(0.0));
        maze_model = glm::scale(maze_model, glm::vec3(1.5));
        import_shader.setMat4("model", maze_model);
        import_shader.setBool("use_texture", true);
        texture_shader.setMat4("transform", glm::mat4(1.0f));
        //glBindTexture(GL_TEXTURE_2D, floor_texture);
        maze.Draw();
        import_shader.setBool("use_texture", false);
        
        // Draws the player
        player.Draw(&import_shader);
        
        // Draws the bullet and updates its position
        if (shot_out){
            bullet.Move();
            bullet.Draw(&import_shader);
        }
        glfwSwapBuffers(window);

        //checks if any events are triggered (keyboard or mouse input)
        glfwPollEvents();
    }

    //Deallocate memory
    std::cout<<"ESC Pressed...Exiting"<<std::endl;
    std::vector<VAOStruct*> all_vaos {&texture_vao,&import_vao};
    for (int i = 0; i < all_vaos.size(); i++) {
        glDeleteVertexArrays(1,&(all_vaos[i]->id));
    }
    std::cout<<"   ...Deallocated VAOs"<<std::endl;
    //terminate your window when you leave the render loop
    glfwTerminate();
    //must have a return value 0 indicates successful run...in this case yes.
    return 0;

}

void processInput(GLFWwindow *window, Player* player, VAOStruct importVAO, ImportOBJ importer, Bullet bullet)
{
    // sets camera type and fires a bullet based on input
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !top_cam){ //alt cam
        top_cam = true;
        main_cam = false;
        third_cam = false;
    }   
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !third_cam){
        third_cam = true;
        main_cam = false;
        top_cam = false;
    }
    if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !main_cam){
        main_cam = true;
        top_cam = false;
        third_cam = false;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS && !shot_out){
        bullet.Shoot(player->location, player->angle_z);
        shot_out = true;
    }
    if(glfwGetKey(window, GLFW_KEY_R)==GLFW_PRESS && !red){
        red = true;
    }
    if(glfwGetKey(window, GLFW_KEY_N)==GLFW_PRESS && red){
        red = false;
    }

    // moves free flying camera
    if (main_cam){
        std::vector<int> keys {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_A};
        std::vector<Camera_Movement> directions {FORWARD, BACKWARD, RIGHT, LEFT};
        for (int i = 0; i<keys.size(); i++){
            if(glfwGetKey(window, keys[i])==GLFW_PRESS){
                curr_camera->ProcessKeyboard(directions[i], 0.001);
            }
        }
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse) {
        first_mouse = false;
        lastX = xpos;
        lastY = ypos;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    curr_camera->ProcessMouseMovement(xoffset,yoffset,true);

}