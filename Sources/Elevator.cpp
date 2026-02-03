#include "../Headers/Elevator.hpp"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <iostream>

Elevator::Elevator(const std::string& liftPath,
    const std::string& doorPath,
    float panelLeft, float panelRight,
    float panelBottom, float panelTop,
    int panelRows, int panelCols,
    float buttonWidth, float buttonHeight,
    float hSpacing, float vSpacing)
    : liftModel(liftPath), doorModel(doorPath),
    position(0.0f, 0.0f, 0.0f),
    totalFloors(panelRows), floorSpacing(3.0f),
    liftFloor(0), liftSpeed(0.01f),
    doorsOpen(false), doorOpenTime(0.0f), doorDuration(5.0f),
    ventilationOn(false),
    panelGrid(panelLeft, panelRight, panelBottom, panelTop,
        panelRows, panelCols,
        buttonWidth, buttonHeight,
        hSpacing, vSpacing,
        1.0f, 0.5f, 0.0f) // boja dugmadi, npr narandžasto
{
    targetFloors.clear();
}

void Elevator::callLift(int floor) {
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end())
        targetFloors.push_back(floor);
}

void Elevator::updateLift(bool personInLift, float mouseX, float mouseY) {
    float now = static_cast<float>(glfwGetTime());

    // Check panel click ako je osoba u liftu
    if (personInLift) {
        panelGrid.checkClick(mouseX, mouseY, true);

        for (auto& btn : panelGrid.getFloorButtons()) {
            if (btn.pressed &&
                std::find(targetFloors.begin(), targetFloors.end(), btn.floor) == targetFloors.end()) {
                targetFloors.push_back(btn.floor);
                btn.pressed = false;
            }
        }
    }

    // Vrata otvorena?
    if (doorsOpen && now - doorOpenTime >= doorDuration)
        doorsOpen = false;

    if (targetFloors.empty()) return;

    int nextFloor = targetFloors.front();

    // pomeranje lifta
    if (nextFloor > liftFloor) liftFloor++;
    else if (nextFloor < liftFloor) liftFloor--;
    else {
        liftFloor = nextFloor;
        doorsOpen = true;
        doorOpenTime = now;
        targetFloors.erase(targetFloors.begin());
    }
}

void Elevator::draw(Shader& shader3D, GLuint shader2D) {
    // Lift 3D
    glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(position.x, liftFloor * floorSpacing, position.z));
    shader3D.setMat4("uM", model);
    liftModel.Draw(shader3D);
    doorModel.Draw(shader3D);

    // Dugmici 2D
    panelGrid.draw(shader2D);
}
