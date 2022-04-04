#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "camera.hpp"
#include "Font.hpp"
#include "import_object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables

float rotation_x = 0.0;
float rotation_z = 0.0;

Camera  camera(glm::vec3(2.0,3.0,7.0),glm::vec3(0.0,1.0,0.0),-90.0f,-25.0f);
Camera  *curr_camera = &camera;

bool first_mouse = true;
int screen_width = 600;
int screen_height = 600;
double lastX = screen_width/2.0;
double lastY = screen_height/2.0;

void render_pyramid (Shader* shader, std::vector<BasicShape*> shapes,
                     unsigned int floor_texture, unsigned int side_texture,
                     glm::vec3 location);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Font arialFont("fonts/ArialBlackLarge.bmp","fonts/ArialBlack.csv",0.3,0.4);

int main () {
    
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",screen_width,screen_height);
    if (window == NULL) {
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback);  


    //Blending....dealing with the alpha channel for transparency.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int wood_texture = GetTexture("./images/wood.jpg");
    unsigned int tile_texture = GetTexture("./images/tile.jpg");
    unsigned int floor_texture = GetTexture("./images/hull_texture.png");

    glm::vec4 dir_light_color = glm::vec4(1.0,1.0,1.0,1.0);
    glm::vec4 dir_light_direction = glm::vec4(1.0,-1.0,0.0,0.0);

    Shader texture_shader("./shaders/textureVertexShader.glsl","./shaders/textureFragmentShader.glsl");
    Shader font_program ("./shaders/textureVertexShader.glsl","./shaders/fontFragmentShader.glsl");
    Shader import_shader("./shaders/importVertexShader.glsl","./shaders/importFragmentShader.glsl");

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
    //Each vertex has a position (x,y,z), surface normal (sx,sy,sz)
    // texture coordinate (s, t), color attribute (r,g,b), 
    // and a specular color (r,g,b)
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

    arialFont.initialize(texture_vao);

    BasicShape pyramid_floor = GetTexturedRectangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),0.5,0.5);
    BasicShape triangle_front = GetTexturedTriangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),
                                                                glm::vec3(0.25,-0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_right = GetTexturedTriangle(texture_vao,glm::vec3(0.25,-0.25,0.0),
                                                                glm::vec3(0.25,0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_back = GetTexturedTriangle(texture_vao,glm::vec3( 0.25,0.25,0.0),
                                                                glm::vec3(-0.25,0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_left = GetTexturedTriangle(texture_vao,glm::vec3( -0.25,0.25,0.0),
                                                                glm::vec3(-0.25,-0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    
    std::vector<BasicShape*> pyramid_shapes {&pyramid_floor,&triangle_back,
                                    &triangle_front,&triangle_left,
                                    &triangle_right};
    
    BasicShape floor = GetTexturedRectangle(texture_vao,glm::vec3(0.0,0.0,0.0),10.0,10.0,10.0);
    
    //Create an import object to import different blender-generated shapes
    ImportOBJ importer;
    BasicShape space_ship = importer.loadFiles("./models/ship",import_vao);
    BasicShape smile = importer.loadFiles("./models/smiley",import_vao);
    BasicShape die = importer.loadFiles("./models/die",import_vao);
    unsigned int die_texture = importer.getTexture();

    //Model Location, Scale, and Textures:
    std::vector<glm::vec3> model_locs {
          glm::vec3(0.0,0.5,0.0),
          glm::vec3(2.0,0.5,0.0),
          glm::vec3(-2.0,0.5,0.0)  
        };

    std::vector<glm::vec3>model_scales {
            glm::vec3(0.2,0.2,0.2),
            glm::vec3(0.5,0.5,0.5),
            glm::vec3(0.2,0.2,0.2)
        };

    std::vector<bool> use_texture {true,false,false};
    std::vector<unsigned int> textures {die_texture,0,0};
    std::vector<BasicShape*>model_shapes{&die,&smile,&space_ship};

    //Set up initial transformations.
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
    glm::mat4 view = glm::mat4(1.0);
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
        shaders[i]->setVec4("direction_light.direction",dir_light_direction);
        shaders[i]->setVec4("direction_light.ambient",0.1f*dir_light_color);
        shaders[i]->setVec4("direction_light.diffuse",dir_light_color);
        shaders[i]->setVec4("direction_light.specular",dir_light_color);
        shaders[i]->setBool("direction_light.on",true);
        shaders[i]->setBool("spot_light.on",false);
    }

        //Set up the font_program shader here.  Note the use of orthographic projection
    // to maintain the "heads up" display look and feel.  You could use perspective
    // as well for a neat effect.
    font_program.use();
   
    font_program.setMat4("view",glm::mat4(1.0));
    font_program.setMat4("model",glm::mat4(1.0));
    font_program.setMat4("transform",glm::mat4(1.0f));
    font_program.setMat4("projection", glm::ortho(-3.0, 3.0, -3.0, 3.0, -1.0, 1.0));
    font_program.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    font_program.setFloat("alpha", 0.3);
    font_program.setInt("texture1", 0);
    
    glEnable(GL_DEPTH_TEST);

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        
        //input
        processInput(window);


        //set the clear color to  wipe the window
        glClearColor(0.0,0.0,0.0,1.0);

        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = curr_camera->GetViewMatrix();
        for (int i = 0; i < shaders.size(); i++) {
            shaders[i]->use();
            shaders[i]->setMat4("view",view);
            shaders[i]->setVec4("eye_position",glm::vec4(camera.Position,1.0));
        }

        texture_shader.use();
        glm::mat4 pyramid_model = glm::mat4(1.0);
        pyramid_model = glm::translate(pyramid_model,glm::vec3(0.0,0.0,-2.0));
        pyramid_model = glm::rotate(pyramid_model,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        pyramid_model = glm::scale(pyramid_model,glm::vec3(2.0,2.0,2.0));
        
        texture_shader.setMat4("model",pyramid_model);
        render_pyramid(&texture_shader,pyramid_shapes,tile_texture,wood_texture,
                            glm::vec3(0.0,0.0,0.0));
        
        glm::mat4 floor_model = glm::mat4(1.0);
        floor_model = glm::rotate(floor_model,glm::radians(90.0f),glm::vec3(1.0,0.0,0.0));
        floor_model = glm::translate(floor_model,glm::vec3(-5.0,-5.0,0.0));
        texture_shader.setMat4("model",floor_model);
        texture_shader.setMat4("transform",glm::mat4(1.0f));
        glBindTexture(GL_TEXTURE_2D,floor_texture);
        floor.Draw();

        

        import_shader.use();
        import_shader.setMat4("transform",glm::mat4(1.0));
        for (int i = 0; i < model_shapes.size(); i++) {
            glm::mat4 shape_model = glm::mat4(1.0);
            
            shape_model = glm::translate(shape_model,model_locs[i]);
            shape_model = glm::rotate(shape_model,glm::radians(rotation_z),glm::vec3(0.0,1.0,0.0));
            shape_model = glm::scale(shape_model,model_scales[i]); 
            import_shader.setMat4("model",shape_model);
            import_shader.setBool("use_texture",use_texture[i]);
            if(use_texture[i]){
                glBindTexture(GL_TEXTURE_2D,textures[i]);
            }
            model_shapes[i]->Draw();
        }
        
        /****Heads up display must be last so that the semi-transparency works***/
        //With blending...order still matters unfortunately.
        font_program.use();
        glm::mat4 font_model = glm::mat4(1.0f);
        font_model = glm::translate(font_model,glm::vec3(-0.3,2.0,0.0));
        font_program.setMat4("model",font_model);

        arialFont.DrawText("Heads up!",glm::vec2(0.0,0.0),font_program);

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
    for (int i = 0; i < pyramid_shapes.size();i++) {
        pyramid_shapes[i]->DeallocateShape();
    }
    for (int i = 0; i < model_shapes.size();i++) {
        model_shapes[i]->DeallocateShape();
    }
    std::cout<<"   ...Deallocated VBOs"<<std::endl;


    //terminate your window when you leave the render loop
    glfwTerminate();
    //must have a return value 0 indicates successful run...in this case yes.
    return 0;

}

void processInput(GLFWwindow *window)
{
    
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) 
        rotation_z+=0.1;
    if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS)
        rotation_z-=0.1;

    std::vector<int> keys {GLFW_KEY_W,GLFW_KEY_S,GLFW_KEY_D,GLFW_KEY_A};
    std::vector<Camera_Movement> directions {FORWARD,BACKWARD,RIGHT,LEFT};
    for(int i = 0; i < keys.size(); i++) {
        if(glfwGetKey(window,keys[i])==GLFW_PRESS)
            curr_camera->ProcessKeyboard(directions[i],0.001);
    }
    

}

void render_pyramid (Shader* shader, std::vector<BasicShape*> shapes,
                     unsigned int floor_texture, unsigned int side_texture,
                     glm::vec3 location) 

{

        glm::mat4 transform = glm::mat4(1.0);

        transform = glm::translate(transform, location);
        transform = glm::rotate(transform,
                                        glm::radians(rotation_x),
                                         glm::vec3(1.0,0.0,0.0));
        transform = glm::rotate(transform,
                                         glm::radians(rotation_z),
                                         glm::vec3(0.0,0.0,1.0));

        
        shader->setMat4("transform",transform);
        glBindTexture(GL_TEXTURE_2D,floor_texture);
        shapes[0]->Draw();
        if (shapes.size()<=1)
           return;
        for (int i = 1; i < shapes.size(); i++) {
            glBindTexture(GL_TEXTURE_2D,side_texture);
            shapes[i]->Draw();
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