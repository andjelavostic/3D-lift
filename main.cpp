//Opis: Primjer ucitavanja modela upotrebom ASSIMP biblioteke
//Preuzeto sa learnOpenGL
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread> // Za FPS limiter
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/shader.hpp"
#include "Headers/model.hpp"

// Parametri ekrana
unsigned int wWidth = 800; // Biće ažurirano na rezoluciju monitora
unsigned int wHeight = 600;

// Parametri kamere
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX, lastY;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Callback za miš
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    cameraPos.y = 3.0f; // Lock na visinu poda
}

int main() {
    if (!glfwInit()) return -1;

    // Dobavljanje primarnog monitora i njegove rezolucije za Full Screen
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    wWidth = mode->width;
    wHeight = mode->height;
    lastX = wWidth / 2.0f;
    lastY = wHeight / 2.0f;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // Preporuka za stabilnost
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // Kreiranje Full Screen prozora
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "3D Sprat - FPS 75", primaryMonitor, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (glewInit() != GLEW_OK) return -3;

    glEnable(GL_DEPTH_TEST);

    Model lija("res/scene.obj");
    Shader unifiedShader("basic.vert", "basic.frag");

    Model liftModel("res/elevator.obj"); 

    // Logika lifta
    float liftY = 0.0f;          // Trenutna visina lifta
    float targetLiftY = 0.0f;    // Visina sprata na koji lift treba da ide
    float liftSpeed = 2.0f;      // Brzina kretanja lifta
    bool doorsOpen = false;      // Logika za vrata

    // Inicijalne matrice
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)wWidth / (float)wHeight, 0.1f, 500.0f);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.07f, 0.07f, 0.07f));

    // FPS Limiter setup (75 FPS)
    const double targetFPS = 75.0;
    const double frameTimeLimit = 1.0 / targetFPS;

    while (!glfwWindowShouldClose(window)) {
        double frameStartTime = glfwGetTime();
        
        // 1. Vreme i DeltaTime
        float currentFrame = (float)frameStartTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 2. Input
        processInput(window);

        // 3. Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unifiedShader.use();
        unifiedShader.setVec3("uLightPos", 5.0f, 10.0f, 5.0f);
        unifiedShader.setVec3("uLightColor", 1.0f, 1.0f, 1.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unifiedShader.setMat4("uP", projection);
        unifiedShader.setMat4("uV", view);
        unifiedShader.setMat4("uM", modelMatrix);
        unifiedShader.setVec3("uViewPos", cameraPos);

        lija.Draw(unifiedShader);

        // 2. Matrica za lift
        glm::mat4 liftM = glm::mat4(1.0f);

        // POZICIJA: Pomeraj treći broj (Z) dok ne upadne u zid. 
        // Ako je zid "ispred" tebe kad kreneš, smanjuj Z (npr. -15, -20...)
        // Ako je "iza", povećavaj Z.
        liftM = glm::translate(liftM, glm::vec3(5.5f,5.5f, -7.5f));
        
        // SKALA: Pošto je 0.07 premalo, probaj npr. 2.5 ili više dok ne popuni rupu u zidu
        liftM = glm::scale(liftM, glm::vec3(0.025f, 0.025f, 0.025f));
        unifiedShader.setMat4("uM", liftM);
        liftModel.Draw(unifiedShader);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS Limiter - uspavaj nit do sledećeg frejma
        double frameEndTime = glfwGetTime();
        double actualFrameTime = frameEndTime - frameStartTime;
        if (actualFrameTime < frameTimeLimit) {
            double sleepTime = frameTimeLimit - actualFrameTime;
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }

    glfwTerminate();
    return 0;
}