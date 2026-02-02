#ifndef FLOOR_H
#define FLOOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "Mesh.hpp"

class Floor {
private:
    Mesh floorMesh;         // Mesh za pod i zidove sprata
    glm::vec3 position;     // Pozicija sprata u prostoru
    float floorHeight;      // Visina sprata

public:
    // Konstruktor: meshPath - putanja do 3D modela sprata
    Floor(const std::string& meshPath, glm::vec3 pos, float height);

    // Crtanje sprata
    void draw(Shader& shader);

    // Pozicija sprata
    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const { return position; }
    float getHeight() const { return floorHeight; }
};

#endif
