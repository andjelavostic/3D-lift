#include "../Headers/Person.hpp"

Person::Person(Mesh* mesh, const glm::vec3& startPos, float moveSpeed)
    : personMesh(mesh), position(startPos), speed(moveSpeed), insideLift(false), liftOffsetY(0.0f)
{
}

void Person::moveForward() { position.z -= speed; }
void Person::moveBackward() { position.z += speed; }
void Person::moveLeft() { position.x -= speed; }
void Person::moveRight() { position.x += speed; }

void Person::setPosition(const glm::vec3& pos) { position = pos; }

void Person::enterLift(float liftY0) {
    insideLift = true;
    liftOffsetY = position.y - liftY0;
}

void Person::exitLift() {
    insideLift = false;
    liftOffsetY = 0.0f;
}

void Person::syncWithLift(float liftY0) {
    if (insideLift) {
        position.y = liftY0 + liftOffsetY;
    }
}

void Person::draw(Shader& shader) {
    if (!personMesh) return;

    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, position);

    shader.setMat4("uM", modelMat);
    personMesh->Draw(shader);
}