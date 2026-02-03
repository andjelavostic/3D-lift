#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <vector>
#include <string>
#include "PanelGrid.hpp"
#include "Model.hpp"
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
    glm::vec3 position;    // x,z pozicija lifta
    PanelGrid panelGrid;   // 2D dugmici unutar lifta

public:
    Elevator(const std::string& liftPath,
        const std::string& doorPath,
        float panelLeft, float panelRight,
        float panelBottom, float panelTop,
        int panelRows, int panelCols,
        float buttonWidth, float buttonHeight,
        float hSpacing, float vSpacing);

    // Logika lifta
    void callLift(int floor);
    void updateLift(bool personInLift, float mouseX, float mouseY);

    // Crtanje
    void draw(Shader& shader3D, GLuint shader2D);

    // Getteri
    int getLiftFloor() const { return liftFloor; }
    bool isDoorsOpen() const { return doorsOpen; }
    float getFloorSpacing() const { return floorSpacing; }
    bool isVentilationOn() const { return ventilationOn; }

    PanelGrid& getPanelGrid() { return panelGrid; }
};

#endif
