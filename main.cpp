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
#include "Headers/Elevator.hpp"

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
bool isPointOnFloor(glm::vec3 p) {
    // 1. Levi vertikalni krak (od X -11 do 0.5, Z 1 do 11)
    bool part1 = (p.x >= -11.5f && p.x <= 0.5f && p.z >= 1.0f && p.z <= 11.5f);

    // 2. Gornji horizontalni krak (od X 0.5 do 11.5, Z -10.5 do 11.5)
    // Napomena: Z -10.5 je onaj donji deo koji si izmerila (11.031, -10.25)
    bool part2 = (p.x > 0.5f && p.x <= 11.5f && p.z >= -10.5f && p.z <= 11.5f);

    return part1 || part2;
}
void processInput(GLFWwindow* window, Elevator& lift) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.5f * deltaTime;

    // Smerovi kretanja (ravno, bez propadanja)
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));

    glm::vec3 nextPos = cameraPos;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) nextPos += forward * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) nextPos -= forward * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) nextPos -= right * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) nextPos += right * cameraSpeed;

    // --- STRIKTNA KOLIZIJA ---
    bool wasIn = lift.isInside(cameraPos);
    bool willBeIn = lift.isInside(nextPos);
    bool onFloor = isPointOnFloor(nextPos);

    if (wasIn != willBeIn) {
        // POKUŠAJ PROLASKA KROZ ZID ILI VRATA
        if (lift.isAtDoor(nextPos) && lift.doorsOpen) {
            cameraPos = nextPos; // Dozvoli ulaz/izlaz kroz otvorena vrata
        }
        else {
            // BLOKADA: Ovde udaraš u zidove (sa strane, otpozadi, ili zatvorena vrata)
        }
    }
    else {
        // KRETANJE UNUTAR ISTOG PROSTORA
        if (willBeIn || onFloor) {
            cameraPos = nextPos;
        }
    }

    // --- VISINA ---
    if (lift.isInside(cameraPos)) {
        cameraPos.y = lift.currentY + 3.0f;
    }
    else {
        cameraPos.y = 3.0f; // Tvoj lift je na 5.3f, pa pretpostavljam da je i sprat tu
    }
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

    Elevator mojLift("res/elevator.obj", glm::vec3(5.5f, 5.3f, -7.5f));

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
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            std::cout << "Trenutna pozicija: X: " << cameraPos.x << " Z: " << cameraPos.z << std::endl;
        }
        double frameStartTime = glfwGetTime();
        
        // 1. Vreme i DeltaTime
        float currentFrame = (float)frameStartTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 2. Input
        processInput(window,mojLift);

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

        mojLift.update(deltaTime);
        mojLift.draw(unifiedShader);
        if (mojLift.isInside(cameraPos)) {
            cameraPos.y = mojLift.currentY + 1.75f; // Kamera prati lift
            // Ograniči kretanje unutar mojLift.minX, maxX itd.
        }

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