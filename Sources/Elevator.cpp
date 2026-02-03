#include "../Headers/Elevator.hpp"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
Elevator::Elevator(const std::string& liftPath,
    const std::string& doorTexPath,
    int panelRows,
    int panelCols)
    : liftModel(liftPath),
    panelGrid(panelRows, panelCols),
    position(0.0f, 0.0f, 0.0f),
    totalFloors(panelRows),
    floorSpacing(3.0f),
    liftFloor(0),
    liftSpeed(0.01f),
    doorsOpen(false),
    doorOpenTime(0.0f),
    doorDuration(5.0f),
    ventilationOn(false),
    doorWidth(0.6f),
    doorHeight(2.2f),
    doorOffset(0.0f),
    // ---- direktna inicijalizacija vrata ----
    leftDoorMesh(
        // verti + indeksi + tekstura
        std::vector<Vertex>{
            {{0, 0, 0}, { 0,0,1 }, { 0,0 }},
            { {0.6f,0,0},{0,0,1},{1,0} },
            { {0.6f,2.2f,0},{0,0,1},{1,1} },
            { {0,2.2f,0},{0,0,1},{0,1} }
        },
        std::vector<unsigned int>{0, 1, 2, 0, 2, 3},
        std::vector<Texture>{TextureFromFile(doorTexPath.c_str(), ".")}
        ),
        rightDoorMesh(
            std::vector<Vertex>{
                {{0, 0, 0}, { 0,0,1 }, { 0,0 }},
                { {0.6f,0,0},{0,0,1},{1,0} },
                { {0.6f,2.2f,0},{0,0,1},{1,1} },
                { {0,2.2f,0},{0,0,1},{0,1} }
        },
        std::vector<unsigned int>{0, 1, 2, 0, 2, 3},
        std::vector<Texture>{TextureFromFile(doorTexPath.c_str(), ".")}
        )
        {
            // --- pozicioniraj panel u liftu ---
            panelGrid.attachToLiftWall(
                glm::vec3(0.95f, 1.4f, -1.0f),
                glm::vec3(-1, 0, 0),
                0.6f,
                0.9f
            );
        }


void Elevator::callLift(int floor)
{
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end())
        targetFloors.push_back(floor);
}

void Elevator::updateLift(bool personInLift,
    glm::vec3 rayOrigin,
    glm::vec3 rayDir)
{
    float now = static_cast<float>(glfwGetTime());

    // --- klik na panel ---
    if (personInLift) {
        int btn = panelGrid.checkClick(rayOrigin, rayDir, true);
        if (btn >= 0 && std::find(targetFloors.begin(), targetFloors.end(), btn) == targetFloors.end())
            targetFloors.push_back(btn);
    }

    // --- vrata ---
    if (doorsOpen && now - doorOpenTime >= doorDuration)
        doorsOpen = false;

    // --- pomeranje lifta ---
    if (targetFloors.empty()) return;

    int nextFloor = targetFloors.front();

    if (nextFloor > liftFloor)
        liftFloor++;
    else if (nextFloor < liftFloor)
        liftFloor--;
    else {
        doorsOpen = true;
        doorOpenTime = now;
        targetFloors.erase(targetFloors.begin());
    }

    // offset vrata (otvaranje/ zatvaranje)
    float openAmount = doorsOpen ? 0.3f : 0.0f;
    doorOffset = openAmount;
}

void Elevator::draw(Shader& shader3D)
{
    glm::mat4 liftModelMat = glm::translate(glm::mat4(1.0f),
        glm::vec3(position.x, liftFloor * floorSpacing, position.z));

    shader3D.use();
    shader3D.setMat4("model", liftModelMat);

    liftModel.Draw(shader3D);

    // --- vrata ---
    glm::mat4 leftDoorMat = glm::translate(glm::mat4(1.0f),
        glm::vec3(position.x - doorOffset, liftFloor * floorSpacing, position.z));
    shader3D.setMat4("model", leftDoorMat);
    leftDoorMesh.Draw(shader3D);

    glm::mat4 rightDoorMat = glm::translate(glm::mat4(1.0f),
        glm::vec3(position.x + doorOffset, liftFloor * floorSpacing, position.z));
    shader3D.setMat4("model", rightDoorMat);
    rightDoorMesh.Draw(shader3D);

    // --- panel ---
    panelGrid.Draw(shader3D);
}
