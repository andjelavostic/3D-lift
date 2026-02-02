#ifndef ELEVATOR_H
#define ELEVATOR_H
#include <vector>
#include "PanelGrid.hpp"  // logika dugmadi
#include "model.hpp"
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
    glm::vec3 position; // x,z lokacija u prostoru

public:
    Elevator(const std::string& liftPath, const std::string& doorPath, int floors = 9, int startFloor = 0, float spacing = 3.0f, glm::vec3 pos = glm::vec3(0.0f));

    // logika
    void callLift(int floor);
    void updateLift(PanelGrid& panelGrid, bool personInLift);

    // crtanje
    void draw(Shader& shader);

    // getteri
    int getLiftFloor() const { return liftFloor; }
    bool isDoorsOpen() const { return doorsOpen; }
    float getFloorSpacing() const { return floorSpacing; }
    bool isVentilationOn() const { return ventilationOn; }
};
#endif