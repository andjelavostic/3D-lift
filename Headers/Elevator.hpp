#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "PanelGrid.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

class Elevator {
private:
    // --- logika lifta ---
    int totalFloors;
    float floorSpacing;
    int liftFloor;
    float liftSpeed;

    bool doorsOpen;
    float doorOpenTime;
    float doorDuration;

    std::vector<int> targetFloors;
    bool ventilationOn;

    // --- 3D prikaz ---
    Model liftModel;
    glm::vec3 position;

    // --- vrata kao Mesh kvadrati ---
    Mesh leftDoorMesh;
    Mesh rightDoorMesh;
    float doorWidth;
    float doorHeight;
    float doorOffset; // koliko se pomeraju kada su otvorena
    Texture doorTexture;

    // --- panel dugmadi ---
    PanelGrid panelGrid;

public:
    Elevator(const std::string& liftPath,
        const std::string& doorTexPath,
        int panelRows,
        int panelCols);

    void callLift(int floor);

    // ray dolazi iz kamere
    void updateLift(bool personInLift,
        glm::vec3 rayOrigin,
        glm::vec3 rayDir);

    void draw(Shader& shader3D);

    // getteri
    int getLiftFloor() const { return liftFloor; }
    bool isDoorsOpen() const { return doorsOpen; }

    PanelGrid& getPanelGrid() { return panelGrid; }
};

#endif
