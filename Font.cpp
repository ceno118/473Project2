#include "Font.hpp"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>

/////////////////////////////// Initialization functions
Font::Font(std::string fontBMP, std::string fontCSV,float scaleX,float scaleY)
{
    this->BMPfilename = fontBMP;
    this->CSVfilename = fontCSV;
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    
}

void Font::initialize(VAOStruct vao) {
    this->vao = vao;
    this->texNumber = GetTexture(this->BMPfilename,true);
    
    std::ifstream infile(this->CSVfilename.c_str());
    std::string line;
    int lineNum = 0;
    while (std::getline(infile, line)) {
        lineNum += 1;
        std::string field = line.substr(0, line.find(","));
        std::string amountStr = line.substr(line.find(",") + 1);
        int amount = (int)strtol(amountStr.c_str(), NULL, 10);
        

        if (lineNum <= 8) {
            switch(lineNum) {
                case 1: this->texWidth = amount; break;
                case 2: this->texHeight = amount; break;
                case 3: this->cellWidth = amount; break;
                case 4: this->cellHeight = amount; break;
                case 5: this->startNum = amount; break;
                case 7: this->fontHeight = amount; break;
            }
        }
        else if (lineNum <= 264) {
            this->charWidth[lineNum - 9] = amount;
        }
    }
    infile.close();

    this->cellsRow = this->texWidth / this->cellWidth;
    this->endNum = std::min(this->startNum - 1 + this->cellsRow * (this->texHeight / this->cellHeight), 255);

    for (int i = 0; i < 256; i++) {
        if (i >= this->startNum && i <= this->endNum) {
            this->genCharShape(&charVAOs[i],(char)i);
        }
    }
}

//Draw a single character, given a single character, a location (x,y) for the
// character, a shader program, and a depth (z).
void Font::DrawCharacter (char letter, glm::vec2 loc, Shader sProgram, float depth_change) {
    sProgram.use();
    glm::mat4 mod = glm::mat4(1.0f);
    mod = glm::translate(mod,glm::vec3(loc.x,loc.y,depth_change));
    mod = glm::scale(mod,glm::vec3(this->scaleX,this->scaleY,1.0f));


    sProgram.setMat4("transform",mod);
    glBindTexture(GL_TEXTURE_2D,this->getTexNum());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(GL_TEXTURE0);

    unsigned char c = static_cast<unsigned char>(letter);
    this->charVAOs[(int)letter].Draw(sProgram);
    //glBindVertexArray(this->charVAOs[(int)letter]);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, this->charVAOs[(int)letter].v_size);


}

//Given a string, draw all the characters to the screen.
void Font::DrawText(std::string s, glm::vec2 start, Shader sProgram) {
    float depth = -0.01;
    for (int i = 0; i < s.length(); i++) {
        unsigned char letter = static_cast<unsigned char>(s[i]);
        double changeAmount = 0.0;
        if (i > 0) {
            unsigned char past_letter = static_cast<unsigned char>(s[i-1]);
            changeAmount = (1.0*this->charWidth[(int)past_letter])/this->cellWidth;
        }
         start.x+=changeAmount*this->scaleX;
         depth+=0.01;
        this->DrawCharacter(s[i],start,sProgram,depth);
    }
}


/////////////////////////////// Setter functions
void Font::setScale(glm::vec2 newScale) {
    if (newScale.x > 0) this->scaleX = newScale.x;
    if (newScale.y > 0) this->scaleY = newScale.y;
}

/////////////////////////////// Getter functions
int Font::getEndNum() {return this->endNum;}
int Font::getStartNum() {return this->startNum;}
unsigned int Font::getTexNum() {return this->texNumber;}
BasicShape Font::getCharShape(int index) {
    if (index < 0 || index > 255) throw std::invalid_argument("Invalid index to Font::getCharVAO");
    return this->charVAOs[index];
}
int Font::getCharWidth(int index) {
    if (index < 0 || index > 255) throw std::invalid_argument("Invalid index to Font::getCharWidth");
    return this->charWidth[index];
}


/////////////////////////////// Private functions

/** Returns texture coordinates for a given character */
glm::vec2 Font::getTexCoords(unsigned char c) {
    int ascii = (int)c;
    int col = ascii % this->cellsRow;
    int row = (ascii - this->startNum) / this->cellsRow;
    double texX = (col * this->cellWidth - 1) / (double)this->texWidth;
    double texY = 1 - (row * this->cellHeight / (double)this->texHeight) ;

    return glm::vec2(texX, texY);
}

void Font::genCharShape(BasicShape *shape, unsigned char c) {
    
    glm::vec2 ulh = this->getTexCoords(c);
    
    glm::vec2 lrh = glm::vec2(ulh.x + ((this->cellWidth) / (double)this->texWidth),
                     ulh.y - (this->fontHeight / (double)this->texHeight));
   

    float aspectRatio = (float)this->fontHeight / this->cellWidth;

    float vertices[] = {
        0.0f, aspectRatio, 0.0f, ulh.x, ulh.y,0.0f,0.0f,1.0f,
        1.0f, aspectRatio, 0.0f, lrh.x, ulh.y,0.0f,0.0f,1.0f,
        1.0f, 0.0f, 0.0f, lrh.x, lrh.y,0.0f,0.0f,1.0f,
        0.0f, 0.0f, 0.0f, ulh.x, lrh.y,0.0f,0.0f,1.0f
    };

    //Shape finalShape;
    shape->Initialize(this->vao,vertices,sizeof(vertices),sizeof(vertices)/sizeof(float)/8.0,GL_TRIANGLE_FAN);
    //set_up_shape(shape, (void*)vertices, sizeof(vertices)/sizeof(float),8,sizeof(float));
    //return finalShape;
}
