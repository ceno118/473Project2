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
#include "maze.hpp"
#include "stb_image.h"
#include <chrono>
#include <ctime>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window, Player* player, Bullet bullet);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void checkPost(bool nvg, std::vector<Shader*> shaders, glm::vec4 dir_light_color);
void checkBullet(bool shot_out, Bullet bullet);
void checkCam(Player* player_pointer);
void mazeDraw(BasicShape floor, BasicShape walls, BasicShape targets, 
    Shader import_shader, unsigned int floor_tex, unsigned int wall_tex, unsigned int target_tex);
void drawSkybox(Shader skybox_shader, unsigned int cubemapTexture, unsigned int skyboxVAO);
void drawHUD(Shader font_program, Font arialFont);

//
// GLOBAL VARIABLES
//

// Camera Selection
bool top_cam = false;
bool third_cam = true;
bool first_cam = false;
bool shot_out = false;

// Post-Processing/Lighting
bool nvg = false;
bool spotlight_on = false;
bool light_pressed = false;

// Show HUD
bool hud = true;
bool hud_pressed = false;

// Stops Timer
bool gameover = false;

// Camera
Camera  camera(glm::vec3(2.0,3.0,7.0),glm::vec3(0.0,1.0,0.0),0.0f,-25.0f);
Camera  *curr_camera = &camera;

// Window Variables
bool first_mouse = true;
int screen_width = 600;
int screen_height = 600;
double lastX = screen_width/2.0;
double lastY = screen_height/2.0;

// Initialization of function to load skybox
unsigned int loadCubemap(vector<std::string> faces);

// Vectors for the location of each target and a vector to hold them.
glm::vec3 target0 = {-6.4, -1.2, 6.17};
glm::vec3 target1 = {-5.2, -1.2, -5.9};
glm::vec3 target2 = {-2.4, -1.2, 2.37};
glm::vec3 target3 = {2.97, -1.2, -0.2};
glm::vec3 target4 = {2.09, -1.2, -4.6};
glm::vec3 target5 = {0.28, -1.2, 1.75};
glm::vec3 target6 = {-2.4, -1.2, -5.7};
std::vector<glm::vec3> target_locs;

// Initial view/project matrix to be transformed.
glm::mat4 view = glm::mat4(1.0);
glm::mat4 project = glm::mat4(1.0);

int main () {
    
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",screen_width,screen_height);
    if (window == NULL) {
        return -1;
    }

    //
    // SETUP
    //

    // Fills target location vector with data.
    target_locs.push_back(target0);
    target_locs.push_back(target1);
    target_locs.push_back(target2);
    target_locs.push_back(target3);
    target_locs.push_back(target4);
    target_locs.push_back(target5);
    target_locs.push_back(target6);
    
    // Finds the time the program was started.
    auto start_time = std::chrono::system_clock::now();
    
    // Variables to control lighting
    float ambient_strength = 0.3;
    glm::vec4 light_color = glm::vec4(1.0);
    glm::vec4 light_position = glm::vec4(0.0, 10.0, 0.0, 1.0);
    glm::vec4 dir_light_color = glm::vec4(1.0,1.0,1.0,1.0);
    glm::vec4 dir_light_direction = glm::vec4(0.5,-1.0,0.0,0.0);
    glm::vec4 green_light_color = glm::vec4(0.0, 1.0, 0.0, 1.0);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);  

    //Blending....dealing with the alpha channel for transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Creates a usable font for text rendering
    Font arialFont("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv", 0.3, 0.4);

    // Creates shaders for different objects and one for text rendering.
    Shader import_shader("./shaders/importVertexShader.glsl","./shaders/importFragmentShader.glsl");
    Shader skybox_shader("./shaders/skyboxVertexShader.glsl", "./shaders/skyboxFragmentShader.glsl");
    Shader font_program("./shaders/textureVertexShader.glsl", "./shaders/fontFragmentShader.glsl");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    // Vertex Array Object for textured VBOs
    // Only used for text rendering in this program
    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer texture_pos_attr = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,8*sizeof(float),3*sizeof(float));
    AttributePointer normal_attr = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),5*sizeof(float));
    texture_vao.attributes.push_back(texture_pos_attr);
    texture_vao.attributes.push_back(texture_attr);
    texture_vao.attributes.push_back(normal_attr);


    // Vertex Array Object for imported VBOs
    // Maze, Player, Bullet
    VAOStruct import_vao;
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


    // Vertices to render the skybox
    float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};
    // Collection of images that make up the skybox
    vector<std::string> faces{
        "./images/right.jpg",
        "./images/left.jpg",
        "./images/top.jpg",
        "./images/bottom.jpg",
        "./images/front.jpg",
        "./images/back.jpg"
    };

    // VAO and VBO to render skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    
    
    
    // Create an import object to import different blender-generated shapes
    ImportOBJ importer;

    // Imports blender objects and initializes them as loadable shapes
    BasicShape floor = importer.loadFiles("./models/floor", import_vao);
    BasicShape walls = importer.loadFiles("./models/walls", import_vao);
    BasicShape targets = importer.loadFiles("./models/target2", import_vao);
    
    // Uses Player class to create player object
    Player player(importer.loadFiles("./models/low_poly", import_vao));
    Player* player_pointer = &player;
    
    // Uses Bullet class to create bullet object
    Bullet bullet(importer.loadFiles("./models/bullet", import_vao), &player);
    Bullet* bullet_pointer = &bullet;
    
    // Initializes font so we can use it to render text
    arialFont.initialize(texture_vao);

    // Loads textures for skybox and maze
    unsigned int cubemapTexture = loadCubemap(faces, false);
    unsigned int wall_tex = GetTexture("./images/plywood.jpg");
    unsigned int floor_tex = GetTexture("./images/concrete.jpg");
    unsigned int target_tex = GetTexture("./images/target5.png");

    //
    // Set up initial transformations for shaders
    //
    
    
    // Font Program
    font_program.use();
    font_program.setMat4("view",glm::mat4(1.0));
    font_program.setMat4("model",glm::mat4(1.0));
    font_program.setMat4("transform",glm::mat4(1.0f));
    font_program.setMat4("projection", glm::ortho(-3.0, 3.0, -3.0, 3.0, -1.0, 1.0));
    font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    font_program.setFloat("alpha", 0);
    font_program.setInt("texture1", 0);

    // Shaders
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
    view = glm::translate(view,glm::vec3(0.0,0.0,-5.0));
    
    project = glm::perspective(glm::radians(45.0f),(1.0f*screen_width)/(1.0f*screen_height),0.1f,100.0f);
    
    std::vector<Shader*> shaders {&import_shader, &skybox_shader};
    for (int i = 0; i < shaders.size(); i++)
    {
        shaders[i]->use();
        shaders[i]->setMat4("transform",glm::mat4(1.0));
        shaders[i]->setMat4("model",model);
        shaders[i]->setMat4("view",view);
        shaders[i]->setMat4("projection",project);
        shaders[i]->setBool("point_light.on",false);
        
        shaders[i]->setBool("use_nvg", false); // Bool for post-processing

        // Direction Light setup
        shaders[i]->setVec4("direction_light.direction",dir_light_direction);
        shaders[i]->setVec4("direction_light.ambient",0.2f*dir_light_color);
        shaders[i]->setVec4("direction_light.diffuse",dir_light_color);
        shaders[i]->setVec4("direction_light.specular",dir_light_color);
        shaders[i]->setBool("direction_light.on",true);

        // Spotlight setup
        shaders[i]->setVec4("spot_light.position", glm::vec4(camera.Position, 1.0));
        shaders[i]->setVec4("spot_light.direction", glm::vec4(camera.Front, 1.0));
        shaders[i]->setFloat("spot_light.cutoff", glm::cos(glm::radians(8.5f)));
        shaders[i]->setFloat("spot_light.outer_cutoff", glm::cos(glm::radians(9.5f)));
        shaders[i]->setVec4("spot_light.ambient", 0.2f*dir_light_color);
        shaders[i]->setVec4("spot_light.diffuse",dir_light_color);
        shaders[i]->setVec4("spot_light.specular",dir_light_color);
        shaders[i]->setFloat("spot_light.constant", 1.0f);
        shaders[i]->setFloat("spot_light.linear", 0.045f);
        shaders[i]->setFloat("spot_light.quadratic", 0.0075f);
        shaders[i]->setBool("spot_light.on",false);
    }

    glEnable(GL_DEPTH_TEST);

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        
        // Checks for user input for player movement, shooting,
        // camera changes, post-processing, and lighting changes.
        player.process_input(window, top_cam);
        processInput(window, &player, bullet);

        // Checks for post-processing boolean
        checkPost(nvg, shaders, dir_light_color);
        
        // Checks if the bullet should be reset
        //checkBullet(shot_out, bullet); I tried implementing this as a separate function but it stopped working when I did.
        if (!shot_out){
            bullet.Reset();
        }
        glm::vec3 bullet_loc = bullet.GetLocation();
        if (bullet_loc.x > 10 || bullet_loc.z > 10 || bullet_loc.x < -10 || bullet_loc.z < -10){
            bullet.Reset();
            shot_out = false;
        }
        for (int i = 0; i<target_locs.size(); i++){
            float t_x = target_locs[i].x;
            float t_y = target_locs[i].y;
            float t_z = target_locs[i].z;

            if (bullet_loc.x < t_x + 0.4 && bullet_loc.x > t_x - 0.4 && bullet_loc.z < t_z + 0.4 && bullet_loc.z > t_z - 0.4){
                bullet.Reset();
                shot_out = false;
                target_locs.erase(target_locs.begin()+i);
            }
        }      

        //set the clear color to wipe the window and wipes it so we can start drawing to the screen
        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Sets camera type/position based on user input
        checkCam(player_pointer);
        
        // Sets view and light position
        for (int i = 0; i < shaders.size(); i++) {
            shaders[i]->use();
            shaders[i]->setMat4("view",view);
            shaders[i]->setVec4("eye_position",glm::vec4(camera.Position,1.0));
            shaders[i]->setVec4("spot_light.position", glm::vec4(player.getLocation().x, player.getLocation().y + 1, player.getLocation().z, 1.0));
            shaders[i]->setVec4("spot_light.direction", glm::vec4(1*(glm::cos(glm::radians(player_pointer->angle_z))), 0, -1*(glm::sin(glm::radians(player_pointer->angle_z))), 1.0f));
            if (spotlight_on){
                shaders[i]->setBool("spot_light.on", true);
            }
            else {
                shaders[i]->setBool("spot_light.on", false);
            }
        }

        //Draws the maze
        mazeDraw(floor, walls, targets, import_shader, floor_tex, wall_tex, target_tex);
        // I wasn't able to get the maze to work as its own class, so I put the code to draw it
        // in a function to clean up the loop
        
        // Draws the player
        player.Draw(import_shader, nvg);
        
        // Draws the bullet and updates its position
        if (shot_out){
            bullet.Move();
            bullet.Draw(&import_shader);
        }
        
        // Draws Skybox
        drawSkybox(skybox_shader, cubemapTexture, skyboxVAO);

        // HUD
        // I used https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
        // to learn about the std::chrono functions to create a timer.
        
        // I wanted to implement this as a function but it crashed when the game ended.
        char elapsed_time[6];
        auto curr_time = std::chrono::system_clock::now();
        if (!gameover){
            std::chrono::duration<double> time_passed = curr_time - start_time;
            snprintf(elapsed_time, sizeof(elapsed_time), "%f", time_passed);
        }
        if (target_locs.size() == 0){
            gameover = true;
        }
        
        if (hud){
            arialFont.DrawText("Time:", glm::vec2(-3, 2.5), font_program);
            arialFont.DrawText(elapsed_time, glm::vec2(-2, 2.5), font_program);
        
            char targets_remaining[3];
            snprintf(targets_remaining, sizeof(targets_remaining), "%d" , target_locs.size());
            arialFont.DrawText("Targets Remaining:", glm::vec2(-3, 2), font_program);
            arialFont.DrawText(targets_remaining, glm::vec2(-0.5, 2), font_program);

            if (nvg){
                arialFont.DrawText("View: NVG", glm::vec2(-3, 1.5), font_program);
            }
            else{
                arialFont.DrawText("View: Default", glm::vec2(-3, 1.5), font_program);
            }
            
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

void processInput(GLFWwindow *window, Player* player, Bullet bullet)
{
    // sets camera type and fires a bullet based on input
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window, GLFW_KEY_H)==GLFW_PRESS && hud && !hud_pressed){
        hud = false;
        hud_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H)==GLFW_PRESS && !hud && !hud_pressed){
        hud = true;
        hud_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE && hud_pressed){
        hud_pressed = false;
    }
    if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !top_cam){ //alt cam
        top_cam = true;
        third_cam = false;
        first_cam = false;
    }   
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !third_cam){
        third_cam = true;
        top_cam = false;
        first_cam = false;
    }
    if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !first_cam){
        first_cam = true;
        third_cam = false;
        top_cam = false;
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS && !shot_out){
        bullet.Shoot(player->location, player->angle_z);
        shot_out = true;
    }
    if(glfwGetKey(window, GLFW_KEY_G)==GLFW_PRESS && !nvg){
        nvg = true;
    }
    if(glfwGetKey(window, GLFW_KEY_N)==GLFW_PRESS && nvg){
        nvg = false;
    }
    if(glfwGetKey(window, GLFW_KEY_F)==GLFW_PRESS && !spotlight_on && !light_pressed){
        spotlight_on = true;
        light_pressed = true;
    }
    if(glfwGetKey(window, GLFW_KEY_F)==GLFW_PRESS && spotlight_on && !light_pressed){
        spotlight_on = false;
        light_pressed = true;
    }
    if(glfwGetKey(window, GLFW_KEY_F)==GLFW_RELEASE && light_pressed){
        light_pressed = false;
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

void checkPost(bool nvg, std::vector<Shader*> shaders, glm::vec4 dir_light_color){
    if (nvg){
           for (int i = 0; i < shaders.size(); i++){
                shaders[i]->use();
                shaders[i]->setBool("use_nvg", true);
                shaders[i]->setVec4("direction_light.ambient",0.2f*dir_light_color);
                shaders[i]->setVec4("ambient", 0.8f*dir_light_color);
           }
        }
        else{
            for (int i = 0; i < shaders.size(); i++){
                shaders[i]->use();
                shaders[i]->setBool("use_nvg", false);
                shaders[i]->setVec4("direction_light.ambient",0.2f*dir_light_color);
                shaders[i]->setVec4("ambient", dir_light_color);
            }
        }
}

void checkBullet(bool shot_out, Bullet bullet){
    if (!shot_out){
        bullet.Reset();
    }
    glm::vec3 bullet_loc = bullet.GetLocation();
    if (bullet_loc.x > 10 || bullet_loc.z > 10 || bullet_loc.x < -10 || bullet_loc.z < -10){
        bullet.Reset();
        shot_out = false;
    }
    for (int i = 0; i<target_locs.size(); i++){
        float t_x = target_locs[i].x;
        float t_y = target_locs[i].y;
        float t_z = target_locs[i].z;

        if (bullet_loc.x < t_x + 0.4 && bullet_loc.x > t_x - 0.4 && bullet_loc.z < t_z + 0.4 && bullet_loc.z > t_z - 0.4){
            bullet.Reset();
            shot_out = false;
            target_locs.erase(target_locs.begin()+i);
        }
    }   
}

void checkCam(Player* player_pointer){
    if (top_cam){// top down on player
            view = glm::lookAt((player_pointer->location + glm::vec3(0,20,0)), player_pointer->location, glm::vec3(0,0,-1));
        }
    else if (third_cam){// third person
            glm::vec3 lookFrom = glm::vec3(-2*(cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 3.5, -2*(sin(glm::radians(-1*(player_pointer->angle_z)))));
            glm::vec3 lookTo = glm::vec3((cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 2.8, (sin(glm::radians(-1*(player_pointer->angle_z)))));
            view = glm::lookAt(player_pointer->location + lookFrom, player_pointer->location + lookTo, glm::vec3(0,1,0));
        }
    else if (first_cam){//first person
            glm::vec3 lookFrom = glm::vec3(0, player_pointer->location.y + 2.3, 0);
            glm::vec3 lookTo = glm::vec3((cos(glm::radians(-1*(player_pointer->angle_z)))), player_pointer->location.y + 2.3, (sin(glm::radians(-1*(player_pointer->angle_z)))));
            view = glm::lookAt(player_pointer->location + lookFrom, player_pointer->location + lookTo, glm::vec3(0,1,0));
        }
    else {// catches any error where none of the camera bools are set
            third_cam = true;
        }
}

void mazeDraw(BasicShape floor, BasicShape walls, BasicShape targets, 
    Shader import_shader, unsigned int floor_tex, unsigned int wall_tex, unsigned int target_tex){
        import_shader.use();
        import_shader.setMat4("transform", glm::mat4(1.0));
        glm::mat4 floor_model = glm::mat4(1.0);
        floor_model = glm::rotate(floor_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
        floor_model = glm::translate(floor_model, glm::vec3(0.0));
        floor_model = glm::scale(floor_model, glm::vec3(1.5));
        import_shader.setMat4("model", floor_model);
        import_shader.setBool("use_texture", true);
        glBindTexture(GL_TEXTURE_2D, floor_tex);
        import_shader.setMat4("transform", glm::mat4(1.0f));
        floor.Draw();

        import_shader.use();
        import_shader.setMat4("transform", glm::mat4(1.0));
        glm::mat4 walls_model = glm::mat4(1.0);
        walls_model = glm::rotate(walls_model,glm::radians(0.0f),glm::vec3(1.0,0.0,0.0));
        walls_model = glm::translate(walls_model, glm::vec3(0.0));
        walls_model = glm::scale(walls_model, glm::vec3(1.5));
        import_shader.setMat4("model", walls_model);
        import_shader.setBool("use_texture", true);
        glBindTexture(GL_TEXTURE_2D, wall_tex);
        import_shader.setMat4("transform", glm::mat4(1.0f));
        walls.Draw();
        
        import_shader.use();
        import_shader.setMat4("transform", glm::mat4(1.0));
        glm::mat4 targets_model = glm::mat4(1.0);
        targets_model = glm::rotate(targets_model,glm::radians(0.0f),glm::vec3(0.0,1.0,0.0));
        targets_model = glm::translate(targets_model, glm::vec3(0.0));
        targets_model = glm::scale(targets_model, glm::vec3(1.5));
        import_shader.setMat4("model", targets_model);
        import_shader.setBool("use_texture", true);
        glBindTexture(GL_TEXTURE_2D, target_tex);
        import_shader.setMat4("transform", glm::mat4(1.0f));
        targets.Draw();
    }

void drawSkybox(Shader skybox_shader, unsigned int cubemapTexture, unsigned int skyboxVAO){
    skybox_shader.use();
    glm::mat4 skybox_view = glm::mat4(glm::mat3(curr_camera -> GetViewMatrix()));
    skybox_shader.setMat4("view", skybox_view);
    skybox_shader.setMat4("projection", project);

    glDepthFunc(GL_EQUAL);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(0);
    skybox_shader.setInt("skybox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skybox_shader.setVec4("direction_light.ambient", glm::vec4(1.0, 0.0, 0.0, 1.0));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
