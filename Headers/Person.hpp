#ifndef FLOOR_H
#define FLOOR_H
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "shader.hpp"


class Person {
private:
    Mesh* personMesh;         // pokazivač na Mesh (poput tvojih dugmadi)
    glm::vec3 position;
    float speed;

    bool insideLift;
    float liftOffsetY;

public:
    // Konstruktor dobija Mesh objekat i početnu poziciju
    Person(Mesh* mesh, const glm::vec3& startPos, float moveSpeed = 0.05f);

    void moveForward();
    void moveBackward();
    void moveLeft();
    void moveRight();

    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const { return position; }

    bool isInsideLift() const { return insideLift; }
    void enterLift(float liftY0);
    void exitLift();
    void syncWithLift(float liftY0);

    void draw(Shader& shader);
};
#endif