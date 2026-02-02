#include "../Headers/PanelGrid.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

PanelGrid::PanelGrid(int numFloors, Mesh* mesh, const glm::vec3& startPos, const glm::vec3& spacing)
{
    buttonMesh = mesh;

    for (int i = 0; i < numFloors; i++) {
        ButtonState btn;
        btn.floor = i;
        btn.pressed = false;
        btn.highlight = false;
        btn.position = startPos + glm::vec3(0.0f, i * spacing.y, 0.0f); // dugmad vertikalno
        btn.scale = spacing; // koristi spacing kao veličinu dugmeta
        floorButtons.push_back(btn);
    }
}

void PanelGrid::pressButton(int floor)
{
    for (auto& btn : floorButtons) {
        if (btn.floor == floor) {
            btn.pressed = true;
            std::cout << "Dugme sprata " << floor << " pritisnuto!\n";
        }
    }
}

// jednostavna AABB provera intersekcije ray-a i dugmeta
bool rayIntersectsBox(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    const glm::vec3& boxMin, const glm::vec3& boxMax)
{
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t0s = (boxMin - rayOrigin) * invDir;
    glm::vec3 t1s = (boxMax - rayOrigin) * invDir;

    glm::vec3 tmin = glm::min(t0s, t1s);
    glm::vec3 tmax = glm::max(t0s, t1s);

    float tNear = std::max(std::max(tmin.x, tmin.y), tmin.z);
    float tFar = std::min(std::min(tmax.x, tmax.y), tmax.z);

    return tNear <= tFar && tFar > 0.0f;
}

void PanelGrid::checkClick(const glm::vec3& rayOrigin, const glm::vec3& rayDir, bool inLift)
{
    if (!inLift) return;

    for (auto& btn : floorButtons) {
        glm::vec3 min = btn.position - btn.scale * 0.5f;
        glm::vec3 max = btn.position + btn.scale * 0.5f;

        if (rayIntersectsBox(rayOrigin, rayDir, min, max)) {
            btn.highlight = true;
            btn.pressed = true;
            std::cout << "Dugme sprata " << btn.floor << " kliknuto!\n";
        }
        else {
            btn.highlight = false;
        }
    }
}

void PanelGrid::draw(Shader& shader)
{
    for (auto& btn : floorButtons) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, btn.position);
        model = glm::scale(model, btn.scale);

        shader.setMat4("uM", model);

        // highlight ili pressed može menjati boju u shaderu
        if (btn.highlight || btn.pressed) {
            shader.setVec3("uColor", 1.0f, 1.0f, 0.0f); // žuto
        }
        else {
            shader.setVec3("uColor", 0.7f, 0.7f, 0.7f); // sivo
        }

        buttonMesh->Draw(shader);
    }
}
