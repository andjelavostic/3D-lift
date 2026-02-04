#ifndef PANEL_GRID_H
#define PANEL_GRID_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"

class PanelGrid {
private:
    int rows, cols;

    Mesh panelMesh;
    std::vector<Texture> buttonTextures;

    glm::mat4 model;
    glm::vec3 normal;

public:
    PanelGrid(int r, int c);

    void attachToLiftWall(glm::vec3 pos,
        glm::vec3 wallNormal,
        float width,
        float height);

    void Draw(Shader& shader);

    // ray picking
    int checkClick(glm::vec3 rayOrigin,
        glm::vec3 rayDir,
        bool inLift);
};
enum PanelButton {
    BTN_SU = 0,
    BTN_PR = 1,
    BTN_1 = 2,
    BTN_2 = 3,
    BTN_3 = 4,
    BTN_4 = 5,
    BTN_5 = 6,
    BTN_6 = 7,
    BTN_CLOSE = 8,
    BTN_OPEN = 9,
    BTN_STOP = 10,
    BTN_FAN = 11
};

#endif
