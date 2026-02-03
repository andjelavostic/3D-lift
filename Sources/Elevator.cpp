#include "../Headers/Elevator.hpp"

Elevator::Elevator(const char* modelPath, glm::vec3 startPos) {
    model = new Model(modelPath);
    position = startPos;
    currentY = startPos.y;
    targetY = startPos.y;
    speed = 2.0f;
    doorsOpen = false;
    doorOpenTime = 0.0f;
    doorDuration = 5.0f;
    doorExtended = false;
    // Postavi granice na osnovu pozicije (npr. kabina je 2x2 metra)
    // Elevator.cpp konstruktor
    /*minX = position.x - (1.5f * 2.5f); // Pola širine modela * skala
    maxX = position.x + (1.5f * 2.5f);
    minZ = position.z - (1.5f * 2.5f);
    maxZ = position.z + (1.5f * 2.5f);*/
    // Na osnovu merenja, kabina je otprilike ovde:
    minX = 3.06f;
    maxX = 8.82f;
    minZ = -10.43f;
    maxZ = -7.02f; // Zadnji zid je na -9, vrata su na -6.5
}
void Elevator::update(float deltaTime) {
    float now = (float)glfwGetTime();

    // Logika tajmera (tvojih 5 sekundi)
    if (doorsOpen) {
        if (now - doorOpenTime >= doorDuration) {
            doorsOpen = false;
        }
        // Glatko otvaranje
        if (doorOpenFactor < 1.0f) doorOpenFactor += deltaTime * 2.0f;
    }
    else {
        // Glatko zatvaranje
        if (doorOpenFactor > 0.0f) doorOpenFactor -= deltaTime * 2.0f;
    }

    // Ograniči faktor između 0 i 1
    doorOpenFactor = glm::clamp(doorOpenFactor, 0.0f, 1.0f);


    // Pomeranje ka cilju (ako postoji sprat u listi)
    if (!targetFloors.empty()) {
        int nextFloor = targetFloors.front();
        float targetY = nextFloor * 10.0f; // Pretpostavka: sprat je visok 10 jedinica

        if (abs(currentY - targetY) > 0.05f) {
            if (currentY < targetY) currentY += speed * deltaTime;
            else currentY -= speed * deltaTime;
        }
        else {
            // Stigli smo na sprat
            currentY = targetY;
            liftFloor = nextFloor;
            openDoors(); // Otvori vrata na 5 sekundi
            targetFloors.erase(targetFloors.begin()); // Izbaci taj sprat iz liste
        }
    }
}

void Elevator::openDoors() {
    doorsOpen = true;
    doorOpenTime = (float)glfwGetTime();
    doorDuration = 5.0f;
    doorExtended = false; // Resetuj mogućnost produžavanja
}

void Elevator::extendDoors() {
    if (doorsOpen && !doorExtended) {
        doorDuration += 5.0f; // Produži za još 5 sekundi
        doorExtended = true;  // Može samo jednom
        std::cout << "Vrata produžena!" << std::endl;
    }
}

void Elevator::closeDoorsImmediately() {
    if (doorsOpen) {
        // Postavljamo vreme tako da tajmer odmah istekne
        doorOpenTime = (float)glfwGetTime() - doorDuration;
    }
}

void Elevator::draw(Shader& shader) {
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(position.x, currentY, position.z));
    modelMat = glm::scale(modelMat, glm::vec3(0.025f)); // Tvoja skala
    shader.setMat4("uM", modelMat);
    model->Draw(shader);
}
bool Elevator::isInside(glm::vec3 p) {
    // Provera da li je igrač unutar te kutije
    return (p.x >= minX && p.x <= maxX && p.z >= minZ && p.z <= maxZ);
}

bool Elevator::isAtDoor(glm::vec3 p) {
    // Vrata su na prednjoj ivici (maxZ), u onom opsegu koji si izmerila (4.3 - 5.9)
    bool atX = (p.x >= 4.3f && p.x <= 5.9f);
    bool atZ = (p.z >= -7.3f && p.z <= -6.8f); // Prag vrata oko prednje ivice
    return atX && atZ;
}

void Elevator::goToFloor(float yHeight) {
    if (!doorsOpen) targetY = yHeight; // Ne kreći se ako su vrata otvorena
}

void Elevator::toggleDoors() {
    if (abs(targetY - currentY) < 0.05f) { // Otvaraj samo kad lift stoji
        doorsOpen = !doorsOpen;
    }
}