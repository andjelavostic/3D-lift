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
    doorOpenFactor = 0.0f;
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
    //setupBarrier();
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
void Elevator::setupBarrier() {
    float vertices[] = {
        // Pozicije           // Normale          // UV
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f
    };
    glGenVertexArrays(1, &barrierVAO);
    glGenBuffers(1, &barrierVBO);
    glBindVertexArray(barrierVAO);
    glBindBuffer(GL_ARRAY_BUFFER, barrierVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void Elevator::draw(Shader& shader) {
    // 1. NACRTAJ GLAVNI LIFT (Ovaj radi)
    glm::mat4 baseMat = glm::mat4(1.0f);
    baseMat = glm::translate(baseMat, glm::vec3(position.x, currentY, position.z));

    glm::mat4 cabinMat = glm::scale(baseMat, glm::vec3(0.025f));
    shader.setMat4("uM", cabinMat);
    model->Draw(shader);



    // 2. NACRTAJ BARIJERU (Koristimo isti model, ali spljeskan)
    if (doorOpenFactor < 0.98f) {
        // Počinjemo od ISTE pozicije gde je i lift
        glm::mat4 barrierMat = baseMat;

        // POMERANJE: 
        // Pošto je model lifta verovatno centriran, moramo ga pomeriti 
        // malo "napred" da bi bio na vratima. Probaj 0.5f ili -0.5f po Z osi.
        barrierMat = glm::translate(barrierMat, glm::vec3(0.0f, -0.55f, 0.2f));

        // SKALIRANJE:
        // Ovde pravimo "ploču". 
        // Koristimo tvoju osnovnu skalu (0.025f) ali je modifikujemo:
        // X: skala lifta * faktor zatvorenosti
        // Y: skala lifta (da bude iste visine)
        // Z: skoro nula (da bude tanak kao papir)
        float currentWidth = 0.02f * (1.0f - doorOpenFactor);
        barrierMat = glm::scale(barrierMat, glm::vec3(currentWidth, 0.025f, 0.001f));

        shader.setMat4("uM", barrierMat);

        // Boja: postavi sivu da se razlikuje od kabine
        shader.setVec3("uColor", 0.6f, 0.6f, 0.6f);

        model->Draw(shader);

        // Reset boje
        shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);
    }
}
bool Elevator::isInside(glm::vec3 p) {
    // Provera da li je igrač unutar te kutije
    return (p.x >= minX && p.x <= maxX && p.z >= minZ && p.z <= maxZ);
}

void Elevator::goToFloor(float yHeight) {
    if (!doorsOpen) targetY = yHeight; // Ne kreći se ako su vrata otvorena
}
bool Elevator::isAtDoor(glm::vec3 p) {
    // 1. Provera X ose: Mora biti šire od samog modela vrata da bi bilo lakše ući
    // Tvoj lift je od 3.06 do 8.82. Vrata su ti na 4.3 do 5.9.
    bool atX = (p.x >= 4.0f && p.x <= 6.2f);

    // 2. Provera Z ose: maxZ ti je -7.02f. 
    // Napravićemo "prozor" od -7.5 (unutar lifta) do -6.5 (napolju na spratu)
    bool atZ = (p.z >= -7.5f && p.z <= -6.5f);

    return atX && atZ;
}

void Elevator::toggleDoors() {
    if (abs(targetY - currentY) < 0.05f) {
        doorsOpen = !doorsOpen;
        if (doorsOpen) {
            // RESTARTUJ TAJMER: Inače će se zatvoriti odmah ako je prošlo 5 sekundi
            doorOpenTime = (float)glfwGetTime();
        }
    }
}
void Elevator::addTargetFloor(int floor) {
    // Proveri da li sprat već postoji u listi da ne duplira
    if (std::find(targetFloors.begin(), targetFloors.end(), floor) == targetFloors.end()) {
        targetFloors.push_back(floor);
        std::cout << "Dodao sprat " << floor << " u listu ciljeva." << std::endl;
    }
}

