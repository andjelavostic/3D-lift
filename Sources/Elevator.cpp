#include "../Headers/Elevator.hpp"
#include <algorithm>
#include <GLFW/glfw3.h>

Elevator::Elevator(const std::string& liftPath,
    const std::string& doorPath,
    const Mesh* buttonMesh,
    int floors,
    int startFloor,
    float spacing,
    glm::vec3 pos)
    : liftModel(liftPath),
    doorModel(doorPath),
    position(pos),
    totalFloors(floors),
    floorSpacing(spacing),
    liftFloor(startFloor),
    liftSpeed(0.01f),
    doorsOpen(false),
    doorOpenTime(0.0f),
    doorDuration(5.0f),
    ventilationOn(false),
    panelGrid(floors, buttonMesh, glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.2f, 0.2f, 0.05f)) // primer pozicije dugmadi unutar lifta
{
    targetFloors.clear();
}

void Elevator::callLift(int floor) {
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end())
        targetFloors.push_back(floor);
}

void Elevator::updateLift(bool personInLift) {
    float now = static_cast<float>(glfwGetTime());
    auto& buttons = panelGrid.getFloorButtons();

    // dodavanje spratova po klikovima dugmadi
    if (personInLift) {
        for (auto& btn : buttons) {
            if (btn.pressed && std::find(targetFloors.begin(), targetFloors.end(), btn.floor) == targetFloors.end()) {
                targetFloors.push_back(btn.floor);
                btn.pressed = false;
            }
        }
    }

    // vrata otvorena?
    if (doorsOpen) {
        if (now - doorOpenTime >= doorDuration)
            doorsOpen = false;
        else
            return; // lift čeka dok su vrata otvorena
    }

    if (targetFloors.empty()) return;

    int nextFloor = targetFloors.front();

    // pomeranje lifta ka sledećem spratu
    if (nextFloor > liftFloor) liftFloor++;
    else if (nextFloor < liftFloor) liftFloor--;
    else {
        liftFloor = nextFloor;
        doorsOpen = true;
        doorOpenTime = now;
        targetFloors.erase(targetFloors.begin());
    }
}

void Elevator::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);

    // Pozicija lifta po visini sprata
    model = glm::translate(model, glm::vec3(position.x, liftFloor * floorSpacing, position.z));
    shader.setMat4("uM", model);

    liftModel.Draw(shader);

    // Vrata lifta
    model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, liftFloor * floorSpacing, position.z));
    shader.setMat4("uM", model);
    doorModel.Draw(shader);

    // Dugmad unutar lifta
    panelGrid.draw(shader);
}
