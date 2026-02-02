#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <vector>
#include "PanelGrid.hpp"  // logika dugmadi
#include "Model.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>

class Elevator {
private:
    // Logika lifta
    int totalFloors;
    float floorSpacing;
    int liftFloor;
    float liftSpeed;
    bool doorsOpen;
    float doorOpenTime;
    float doorDuration;
    std::vector<int> targetFloors;
    bool ventilationOn;

    // 3D prikaz
    Model liftModel;
    Model doorModel;
    glm::vec3 position;    // x,z lokacija lifta
    PanelGrid panelGrid;   // dugmići unutar lifta

public:
    // Konstruktor: prosledjuj mesh za dugmice
    Elevator(const std::string& liftPath,
        const std::string& doorPath,
        const Mesh* buttonMesh,
        int floors = 9,
        int startFloor = 0,
        float spacing = 3.0f,
        glm::vec3 pos = glm::vec3(0.0f));

    // Logika lifta
    void callLift(int floor);
    void updateLift(bool personInLift);

    // Crtanje
    void draw(Shader& shader);

    // Getteri
    int getLiftFloor() const { return liftFloor; }
    bool isDoorsOpen() const { return doorsOpen; }
    float getFloorSpacing() const { return floorSpacing; }
    bool isVentilationOn() const { return ventilationOn; }

    // PanelGrid getter (ako treba spolja)
    PanelGrid& getPanelGrid() { return panelGrid; }
};

#endif
