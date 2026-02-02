#ifndef PANEL_GRID_H
#define PANEL_GRID_H

#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "shader.hpp"

// Struktura za stanje dugmeta
struct ButtonState {
    int floor;               // koji sprat dugme predstavlja
    bool pressed;            // da li je dugme pritisnuto
    bool highlight;          // da li je uokvireno / highlight
    glm::vec3 position;      // pozicija dugmeta u 3D prostoru
    glm::vec3 scale;         // veličina dugmeta
};

class PanelGrid {
private:
    std::vector<ButtonState> floorButtons;
    Mesh* buttonMesh;  // Mesh dugmeta, svi dugmići koriste isti mesh

public:
    PanelGrid(int numFloors, Mesh* mesh, const glm::vec3& startPos, const glm::vec3& spacing);

    // pritisak dugmeta (logika)
    void pressButton(int floor);

    // provera da li igrač klikće dugme (ray u world prostoru)
    void checkClick(const glm::vec3& rayOrigin, const glm::vec3& rayDir, bool inLift);

    // crta sve dugmiće
    void draw(Shader& shader);

    // dobij sve dugmiće
    std::vector<ButtonState>& getFloorButtons() { return floorButtons; }

    ~PanelGrid() {}
};

#endif
