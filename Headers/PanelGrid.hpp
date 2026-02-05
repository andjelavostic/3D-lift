#ifndef PANEL_GRID_H
#define PANEL_GRID_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"

struct PanelButton {
    int floorIndex;
    bool active = false;
};
class PanelGrid {
private:
    int rows, cols;

    Mesh panelMesh;
    std::vector<Texture> buttonTextures;

    glm::mat4 model;
    glm::vec3 normal;


public:
    std::vector<PanelButton> buttons;
    PanelGrid(int r, int c);

    void attachToLiftWall(glm::vec3 pos,
        glm::vec3 wallNormal,
        float width,
        float height);

    void Draw(Shader& shader);

    int getButtonAtRay(glm::vec3 rayOrigin, glm::vec3 rayDir);

    // ray picking
    int checkClick(glm::vec3 rayOrigin,
        glm::vec3 rayDir,
        bool inLift);
};


#endif
