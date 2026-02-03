#ifndef FLOOR_H
#define FLOOR_H

#include <glm/glm.hpp>
#include <string>

#include "Shader.hpp"
#include "Model.hpp"

class Floor {
private:
    Model floorModel;      // 3D model sprata (pod + zidovi)
    glm::vec3 position;    // Pozicija sprata u svetu
    float floorHeight;     // Visina sprata

public:
    // Konstruktor: path do 3D modela sprata
    Floor(const std::string& modelPath,
        glm::vec3 pos,
        float height);

    void draw(Shader& shader);

    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition() const { return position; }
    float getHeight() const { return floorHeight; }
};

#endif
