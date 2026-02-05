#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "model.hpp"

class Elevator {
public:
    glm::vec3 position;
    float currentY;
    float targetY;
    float speed;
    bool doorsOpen;
    Model* model;
    float doorOpenTime;
    float doorDuration;
    bool doorExtended;
    int liftFloor;
    std::vector<int> targetFloors;
    unsigned int barrierVAO, barrierVBO;
    void setupBarrier(); // Funkcija koju ćemo pozvati u konstruktoru
    float doorOpenFactor;
    // Granice za koliziju (unutrašnjost kabine)
    float minX, maxX, minZ, maxZ;
    float liftBaseY = 5.3f; // početna visina sprata 0
    float floorHeight = 6.9f; // visina između spratova

    Elevator(const char* modelPath, glm::vec3 startPos);
    void draw(Shader& shader);
    bool isInside(glm::vec3 personPos);
    bool isAtDoor(glm::vec3 p);
    void goToFloor(float yHeight);
    void toggleDoors();
    void update(float deltaTime);
    void openDoors();
    void extendDoors(); // Dugme za produžavanje
    void closeDoorsImmediately();
    void addTargetFloor(int floor); // Dodavanje sprata na koji lift treba da ide
    enum class ElevatorState {
        IDLE,
        MOVING,
        DOORS_OPEN
    };

    ElevatorState state = ElevatorState::IDLE;

};

#endif