#include "../Headers/PanelGrid.hpp"
#include "../Headers/model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

PanelGrid::PanelGrid(int r, int c)
    : rows(r), cols(c),
    panelMesh(
        {
            {{0, 0, 0}, {0, 0, 1}, {0, 1}}, // donja leva → gornja leva
            {{1, 0, 0}, {0, 0, 1}, {1, 1}}, // donja desna → gornja desna
            {{1, 1, 0}, {0, 0, 1}, {1, 0}}, // gornja desna → donja desna
            {{0, 1, 0}, {0, 0, 1}, {0, 0}}  // gornja leva → donja leva
        },
        { 0, 1, 2, 0, 2, 3 },
        {}
    
    )
{
    // Dugmad – REDOSLED = VIZUELNI REDOSLED
    std::vector<std::string> paths = {
        "res/su.png", "res/pr.png",
        "res/number-one.png", "res/number-2.png",
        "res/number-3.png", "res/number-four.png",
        "res/number-five.png", "res/number-six.png",
        "res/close.png", "res/open.png",
        "res/stop.png", "res/fan.png"
    };

    for (auto& p : paths) {
        Texture t;
        t.id = TextureFromFile(p.c_str(), ".");
        t.type = "uDiffMap1";
        t.path = p;
        buttonTextures.push_back(t);
    }
    for (int i = 0; i < rows * cols; i++) {
        buttons.push_back(PanelButton{ i, false });
    }

}
void PanelGrid::attachToLiftWall(Elevator* lift, glm::vec3 offset, glm::vec3 wallNormal, float width, float height)
{
    attachedLift = lift;
    liftOffset = offset;

    normal = glm::normalize(wallNormal);

    model = glm::mat4(1.0f); // početna model matrica = identity

    // sada pozicija i rotacija ostaju iste, ali se offset dodaje u Draw
    // LEVI ZID (X-)
    if (normal.x < -0.5f) {
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    }
    else if (normal.x > 0.5f) {
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    }
    else if (normal.z < -0.5f) {
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
    }
    // Z+ nema rotacije

    model = glm::scale(model, glm::vec3(width, height, 1.0f));
}
void PanelGrid::pressButton(int buttonIndex)
{
    if (!attachedLift) return;

    // Dugmad rade SAMO ako su vrata otvorena
    if (attachedLift->state != Elevator::ElevatorState::DOORS_OPEN)
        return;

    // ---------- SPRATOVI ----------
    if (buttonIndex >= 0 && buttonIndex <= 7) {
        buttons[buttonIndex].active = true;
        attachedLift->addTargetFloor(buttonIndex);
        return;
    }

    // ---------- CLOSE ----------
    if (buttonIndex == 8) {
        attachedLift->closeDoorsImmediately();
        return;
    }

    // ---------- OPEN ----------
    if (buttonIndex == 9) {
        attachedLift->extendDoors();
        return;
    }
}


void PanelGrid::Draw(Shader& shader)
{
    shader.use();
    shader.setInt("uDiffMap1", 0);

    glm::mat4 baseModel = model;

    // Ako je panel povezan sa liftom, dodaj liftovu poziciju
    if (attachedLift) {
        glm::vec3 liftWorldPos(
            attachedLift->position.x,
            attachedLift->currentY,
            attachedLift->position.z
        );

        baseModel =
            glm::translate(glm::mat4(1.0f), liftWorldPos + liftOffset)
            * baseModel;
    }

    float cellW = 1.0f / cols;
    float cellH = 1.0f / rows;
    float buttonScale = 0.75f;

    int i = 0;

    for (int r = rows - 1; r >= 0; r--) {
        for (int c = 0; c < cols; c++) {
            if (i >= buttonTextures.size()) return;

            glm::mat4 m = baseModel;

            // pozicija ćelije
            m = glm::translate(m, glm::vec3(c * cellW, r * cellH, 0.002f));

            // centriranje dugmeta
            float offX = (cellW * (1.0f - buttonScale)) * 0.5f;
            float offY = (cellH * (1.0f - buttonScale)) * 0.5f;
            m = glm::translate(m, glm::vec3(offX, offY, 0.0f));

            // skaliranje dugmeta
            m = glm::scale(m, glm::vec3(cellW * buttonScale, cellH * buttonScale, 1.0f));

            shader.setMat4("uM", m);

            if (buttons[i].active)
                shader.setVec3("uColor", 1.0f, 1.0f, 0.0f);
            else
                shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buttonTextures[i].id);

            panelMesh.Draw(shader);

            i++;
        }
    }
}

int PanelGrid::getButtonAtRay(glm::vec3 rayOrigin, glm::vec3 rayDir) {
    // 1️⃣ Kreiraj globalnu matricu panela (isto kao u Draw)
    glm::mat4 baseModel = model;
    if (attachedLift) {
        glm::vec3 liftWorldPos(
            attachedLift->position.x,
            attachedLift->currentY,
            attachedLift->position.z
        );

        baseModel =
            glm::translate(glm::mat4(1.0f), liftWorldPos + liftOffset)
            * baseModel;

    }

    // 2️⃣ Invertuj globalnu matricu da ray prebaciš u lokalni prostor panela
    glm::mat4 invM = glm::inverse(baseModel);
    glm::vec3 localRayOrigin = glm::vec3(invM * glm::vec4(rayOrigin, 1.0f));
    glm::vec3 localRayDir = glm::normalize(glm::vec3(invM * glm::vec4(rayDir, 0.0f)));

    // provera paralelnosti sa panelom (z=0 u lokalnom prostoru)
    if (fabs(localRayDir.z) < 1e-6f) return -1;

    // nađi gde ray udara u panel
    float t = -localRayOrigin.z / localRayDir.z;
    if (t < 0) return -1; // iza kamere

    glm::vec3 hitPoint = localRayOrigin + t * localRayDir;

    // dugmad su raspoređena od 0 do 1 u X/Y
    float cellW = 1.0f / cols;
    float cellH = 1.0f / rows;

    int col = (int)(hitPoint.x / cellW);
    int row = (int)(hitPoint.y / cellH);

    // proveri da li je u granicama
    if (col < 0 || col >= cols || row < 0 || row >= rows) return -1;

    // raspored: gore→dole, levo→desno
    return (rows - 1 - row) * cols + col;
}
std::vector<glm::vec3> PanelGrid::getActiveLightPositions(glm::mat4 panelModel) {
    std::vector<glm::vec3> lights;
    float cellW = 1.0f / cols;
    float cellH = 1.0f / rows;
    float buttonScale = 0.75f;

    int i = 0;
    for (int r = rows - 1; r >= 0; r--) {
        for (int c = 0; c < cols; c++) {
            if (i >= buttons.size()) break;
            if (buttons[i].active) {
                // pozicija dugmeta u lokalnom prostoru panela
                glm::vec3 localPos(
                    c * cellW + (cellW * buttonScale) * 0.5f,
                    r * cellH + (cellH * buttonScale) * 0.5f,
                    0.01f  // malo ispred panela
                );
                // transformiši u globalne koordinate
                glm::vec3 worldPos = glm::vec3(panelModel * glm::vec4(localPos, 1.0f));
                lights.push_back(worldPos);
            }
            i++;
        }
    }
    return lights;
}
glm::mat4 PanelGrid::getWorldModel() const {
    glm::vec3 liftWorldPos(
        attachedLift->position.x,
        attachedLift->currentY,
        attachedLift->position.z
    );

    return glm::translate(glm::mat4(1.0f), liftWorldPos + liftOffset) * model;
}

