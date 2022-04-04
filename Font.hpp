#ifndef FONT_HPP
#define FONT_HPP

#include "build_shapes.hpp"
#include "basic_shape.hpp"
#include "Shader.hpp"

#include <string>
#include <glm/glm.hpp>

class Font
{
    public:
        //Create the Font object given image and csv files as well as the scaling in the x and y direction.
        Font(std::string fontBMP, std::string fontCSV,float scaleX=1.0f,float scaleY=1.0f);

        //Creates all the Shapes used to create each character (one shape per letter mapped to specific coordinates 
        //in the bitmap image).
        void initialize(VAOStruct vao);

        //Draws a single character at a given x and y coordinate (lower left hand)
        void DrawCharacter (char letter, glm::vec2 loc, Shader sProgram, float depth_change = 0);
        // Draws the string starting at a given X/Y coordinate (lower left hand)
        void DrawText(std::string s, glm::vec2 start, Shader sProgram);

        //Re-scale the characters.
        void setScale(glm::vec2 newScale);

        //Get the last ascii character
        int getEndNum();
        //Get the first
        int getStartNum();
        //Get the texture used for the font (the bitmap file)
        unsigned int getTexNum();

        //Get a single character shape
        BasicShape getCharShape(int index);

        //Get the width of a given character.
        int getCharWidth(int index);


    private:
        VAOStruct vao;
        glm::vec2 getTexCoords(unsigned char c);
        void genCharShape(BasicShape *shape, unsigned char c);

        std::string BMPfilename;
        std::string CSVfilename;

        float scaleX;
        float scaleY;
        BasicShape charVAOs[256];
        int charWidth[256] = {0};

        int texWidth;
        int texHeight;
        int cellWidth;
        int cellHeight;
        int cellsRow;       // Number of cells per row
        int fontHeight;
        int startNum;       // Lowest ASCII number covered
        int endNum;         // Higher ASCII number covered

        unsigned int texNumber;     // OpenGL Texture # for the bitmap font
};

#endif // FONT_HPP
