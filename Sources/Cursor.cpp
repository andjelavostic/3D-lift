#include "../Headers/Cursor.hpp"
#include <iostream>

// ======================
// SHADER SOURCE
// ======================
static const char* vertexSrc = R"(#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* fragmentSrc = R"(#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0); // bela strelica
}
)";

// ======================
// KONSTRUKTOR / DESTRUKTOR
// ======================
Cursor::Cursor() : VAO(0), VBO(0), shaderProgram(0) {}

Cursor::~Cursor() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

// ======================
// INIT
// ======================
void Cursor::init() {

    // Strelica u NDC (uvek u centru)
    float vertices[] = {
        // vertikalna linija (kraća)
        0.0f, -0.02f,
        0.0f,  0.03f,

        // levo krilo
        0.0f, 0.03f,
       -0.015f, 0.015f,

       // desno krilo
       0.0f, 0.03f,
       0.015f, 0.015f
    };


    shaderProgram = createShader();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float), (void*)0
    );
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// ======================
// RENDER
// ======================
void Cursor::render() {
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

// ======================
// SHADER KREACIJA
// ======================
unsigned int Cursor::createShader() {
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
