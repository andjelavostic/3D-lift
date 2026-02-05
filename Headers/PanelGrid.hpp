#ifndef PANEL_GRID_H
#define PANEL_GRID_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "Elevator.hpp"

struct PanelButton {
    int floorIndex;
    bool active = false;
    glm::vec3 localPos;  // pozicija dugmeta u lokalnim koordinatama panela
};
class PanelGrid {
public:
    int rows, cols;

    Mesh panelMesh;
    std::vector<Texture> buttonTextures;

    glm::mat4 model;
    glm::vec3 normal;
    std::vector<Texture> buttonTexturesActive; 


    std::vector<PanelButton> buttons;
    PanelGrid(int r, int c);

    Elevator* attachedLift = nullptr;   // lift na koji je panel pričvršćen
    glm::vec3 liftOffset = glm::vec3(0); // offset u odnosu na lift

    void attachToLiftWall(Elevator* lift, glm::vec3 offset, glm::vec3 wallNormal, float width, float height);

    void pressButton(int buttonIndex);

    void Draw(Shader& shader);

    int getButtonAtRay(glm::vec3 rayOrigin, glm::vec3 rayDir);

    std::vector<glm::vec3> getActiveLightPositions(glm::mat4 panelModel);

    glm::mat4 getWorldModel() const;

    // ray picking
    int checkClick(glm::vec3 rayOrigin,
        glm::vec3 rayDir,
        bool inLift);
    

};


#endif
