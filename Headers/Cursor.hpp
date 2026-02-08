#ifndef CURSOR_HPP
#define CURSOR_HPP
#include <GL/glew.h> 

class Cursor {
public:
    Cursor();
    ~Cursor();

    void init();     // pozvati jednom
    void render();   // pozivati svaki frame

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shaderProgram;

    unsigned int createShader();
};

#endif
