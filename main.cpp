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
#include "Headers/PanelGrid.hpp"
#include "Headers/FloorLabels.hpp"

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

int currentPlayerFloor = 0;


// Callback za miš
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float centerX = wWidth / 2.0;
    float centerY = wHeight / 2.0;

    float xoffset = xpos - centerX;
    float yoffset = centerY - ypos; // y je obrnuto u OpenGL

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

    // Resetuj kursorsku poziciju u centar
    glfwSetCursorPos(window, centerX, centerY);
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

    // --- INTERAKCIJA: TASTER 'C' ---
    static bool cWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!cWasPressed) {

            // 1️⃣ Izračunaj sprat na kome je čovek
            int callerFloor = currentPlayerFloor;
            float callerY = callerFloor * 6.7f;

            bool liftIsHere = fabs(lift.currentY - callerY) < 5.31f;
            std::cout << "Pozvan lift na sprat " << fabs(lift.currentY - callerY) << std::endl;

            // 3️⃣ Proveri da li smo blizu vrata lifta
            float dist = glm::distance(
                glm::vec3(cameraPos.x, 0, cameraPos.z),
                glm::vec3(lift.position.x, 0, lift.position.z)
            );

            bool nearLift = dist < 4.0f;

            // ===============================
            // SLUČAJ 1: UNUTRA
            // ===============================
            if (lift.isInside(cameraPos)) {
                if (liftIsHere)
                    lift.toggleDoors();
            }

            // ===============================
            // SLUČAJ 2: ISPRED LIFTA I TU JE
            // ===============================
            else if (nearLift && liftIsHere) {
                lift.openDoors();
            }

            // ===============================
            // SLUČAJ 3: ISPRED LIFTA, NIJE TU
            // ===============================
            else if (nearLift && !liftIsHere) {
                lift.addTargetFloor(callerFloor);
                std::cout << "Pozvan lift na sprat " << callerFloor << std::endl;
            }

            cWasPressed = true;
        }
    }
    else {
        cWasPressed = false;
    }

    // --- STRIKTNA KOLIZIJA SA MARGINOM---
    // Provera da li je trenutna i buduća pozicija unutar lifta sa marginom
    auto isInsideStrict = [&](glm::vec3 pos) {
        return (pos.x >= lift.minX + 1.0f && pos.x <= lift.maxX - 2.5f &&
            pos.z >= lift.minZ  && pos.z <= lift.maxZ);
        };

    bool wasIn = lift.isInside(cameraPos); // Koristimo običan za detekciju stanja
    bool nextInStrict = isInsideStrict(nextPos);
    bool onFloor = isPointOnFloor(nextPos);

    if (wasIn) {
        // --- AKO SMO U LIFTU ---
        if (nextInStrict) {
            cameraPos = nextPos; // Slobodno kretanje unutar "zatvora"
        }
        else {
            // Pokušaj izlaska - dozvoli samo na vratima
            if (lift.isAtDoor(nextPos) && lift.doorsOpen) {
                cameraPos = nextPos;
            }
            // Inače: Ne menjaj cameraPos (udaraš u unutrašnji zid)
        }
    }
    else {
        // --- AKO SMO VAN LIFTA ---
        if (!lift.isInside(nextPos)) { // Ovde koristimo običan isInside da ne bismo zapeli za spoljašnji zid
            if (onFloor) cameraPos = nextPos;
        }
        else {
            // Pokušaj ulaska
            if (lift.isAtDoor(nextPos) && lift.doorsOpen) {
                cameraPos = nextPos;
            }
            // Inače: Spoljašnji zidovi lifta su neprobojni
        }
    }

    // --- VISINA ---
    if (lift.isInside(cameraPos)) {
        cameraPos.y = lift.currentY - 1.5f; // visina očiju
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (glewInit() != GLEW_OK) return -3;

    glEnable(GL_DEPTH_TEST);

    std::vector<std::string> floorLabelPaths = {
    "res/su.png",              // PR
    "res/number-one.png",
    "res/number-2.png",
    "res/number-3.png",
    "res/number-four.png",
    "res/number-five.png",
    "res/number-six.png"
    };

    FloorLabels floorLabels(floorLabelPaths);
    Model lija("res/scene.obj");
    Shader unifiedShader("basic.vert", "basic.frag");

    Elevator mojLift("res/elevator.obj", glm::vec3(5.5f, 5.3f, -7.5f));
    Shader panelShader("panel.vert", "panel.frag");

    PanelGrid panel(6,2); // 4 reda, 3 kolone

   

    Model lampLift("res/lamp/scene.obj");
    Model lampFloor("res/lamp/scene.obj");


    // Offset relativno na lift
    // LOKALNI offset panela u kabini lifta
    glm::vec3 panelOffset = glm::vec3(
        -1.75f,   // X: levi zid lifta
        -2.5f,   // Y: visina panela od poda
        -1.1f   // Z: malo ka unutra
    );


    // LOKALNI offset lampe u odnosu na centar lifta
    glm::vec3 lampOffset = glm::vec3(
        0.0f,   // X: centar lifta
        0.2f,   // Y: plafon kabine
        -2.0f    // Z: sredina kabine
    );

    Model plant1("res/plants/prva/scene.obj");
    Model plant2("res/plants/druga/scene.obj");
    Model plant3("res/plants/treca/scene.obj");


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

    // --- Spratovi ---
    const int numFloors = 8;
    const float floorHeight = 6.7f;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            std::cout << "Trenutna pozicija: X: " << cameraPos.x << " Y: " << cameraPos.y << std::endl;
        }
        double frameStartTime = glfwGetTime();
        
        // 1. Vreme i DeltaTime
        float currentFrame = (float)frameStartTime;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 2. Input
        processInput(window,mojLift);

        // --- PROVERA KLIKA NA PANEL ---
        static bool mouseWasPressed = false;
        bool mouseClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (mouseClick && !mouseWasPressed && mojLift.isInside(cameraPos)) {
            int btn = panel.getButtonAtRay(cameraPos, cameraFront);
            if (btn != -1) {
                panel.buttons[btn].active = true;
                mojLift.addTargetFloor(btn);
            }
        }
        mouseWasPressed = mouseClick;

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
        unifiedShader.setVec3("uLightPos", glm::vec3(3.0f, 6.2f, 2.0f)); // lampFloor
        unifiedShader.setVec3("uLightColor", glm::vec3(1.0f, 0.9f, 0.7f)); // npr toplo svetlo
        // LampFloor
        unifiedShader.setVec3("uLampPos", glm::vec3(3.0f, 6.2f, 2.0f));
        unifiedShader.setVec3("uLampColor", glm::vec3(1.0f, 0.9f, 0.7f));

        // LampLift
        glm::vec3 liftLampWorldPos(
            mojLift.position.x + lampOffset.x,
            mojLift.currentY + lampOffset.y,
            mojLift.position.z + lampOffset.z
        );

        unifiedShader.setVec3("uLiftLampPos", liftLampWorldPos);
        unifiedShader.setVec3("uLiftLampColor", glm::vec3(1.0f, 0.9f, 0.7f));

        for (int i = 0; i < numFloors; i++) {

            float y = i * floorHeight;

            // =====================
            // SPRAT (scene.obj)
            // =====================
            glm::mat4 floorM = glm::mat4(1.0f);
            floorM = glm::translate(floorM, glm::vec3(0.0f, y, 0.0f));
            floorM = floorM * modelMatrix;

            unifiedShader.setMat4("uM", floorM);
            lija.Draw(unifiedShader);

            // =====================
            // BILJKE
            // =====================
            glm::vec3 plantBasePos(-3.5f, y, 2.0f);
            float plantSpacing = 1.3f;

            glm::mat4 plant1M = glm::translate(glm::mat4(1.0f), plantBasePos);
            plant1M = glm::scale(plant1M, glm::vec3(0.9f));
            unifiedShader.setMat4("uM", plant1M);
            plant1.Draw(unifiedShader);

            glm::mat4 plant2M = glm::translate(glm::mat4(1.0f),
                plantBasePos + glm::vec3(plantSpacing, 0.0f, 0.0f));
            plant2M = glm::scale(plant2M, glm::vec3(0.9f));
            unifiedShader.setMat4("uM", plant2M);
            plant2.Draw(unifiedShader);

            glm::mat4 plant3M = glm::translate(glm::mat4(1.0f),
                plantBasePos + glm::vec3(2 * plantSpacing, 0.0f, 0.0f));
            plant3M = glm::scale(plant3M, glm::vec3(0.01f));
            unifiedShader.setMat4("uM", plant3M);
            plant3.Draw(unifiedShader);

            // =====================
            // LAMPA NA SPRATU
            // =====================
            glm::mat4 lampM = glm::translate(glm::mat4(1.0f),
                glm::vec3(3.0f, y + 6.7f, 2.0f));
            lampM = glm::scale(lampM, glm::vec3(1.5f));
            unifiedShader.setMat4("uM", lampM);
            lampFloor.Draw(unifiedShader);

            floorLabels.Draw(
                unifiedShader,
                numFloors,
                floorHeight,
                glm::vec3(
                    -2.0f,   // X – isto kao biljke
                    2.5f,    // Y – visina nalepnice unutar sprata
                    0.03f     // Z – malo ispred biljaka
                ),
                glm::vec3(0, 0, -1), // normal zida (zadnji zid)
                0.9f
            );


        }


        mojLift.update(deltaTime);
        for (int i = 0; i < panel.buttons.size(); i++) {
            if (panel.buttons[i].active && mojLift.liftFloor == i) {
                panel.buttons[i].active = false; // dugme ugasimo
            }
        }

        mojLift.draw(unifiedShader);
        panel.attachToLiftWall(
            &mojLift,
            panelOffset,              // LOKALNI offset
            glm::vec3(-1, 0, 0),       // levi zid lifta
            0.6f,                      // širina panela
            1.5f                       // visina panela
        );

        panel.Draw(unifiedShader);
        // Svetla dugmadi
        glm::mat4 panelWorldModel = panel.getWorldModel();
        auto panelLights = panel.getActiveLightPositions(panelWorldModel);


        for (int i = 0; i < panelLights.size(); i++) {
            unifiedShader.setVec3("uExtraLights[" + std::to_string(i) + "]", panelLights[i]);
            unifiedShader.setVec3("uExtraLightColors[" + std::to_string(i) + "]", glm::vec3(1.0f, 1.0f, 0.7f));
        }
        unifiedShader.setInt("uNumExtraLights", (int)panelLights.size());
        // Crtanje lampi   
       
        glm::mat4 lampLiftM = glm::translate(glm::mat4(1.0f), liftLampWorldPos);
        lampLiftM = glm::scale(lampLiftM, glm::vec3(1.2f));

        unifiedShader.setMat4("uM", lampLiftM);
        lampLift.Draw(unifiedShader);


        if (!mojLift.isInside(cameraPos)) {
            currentPlayerFloor = (int)round(cameraPos.y / floorHeight);
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