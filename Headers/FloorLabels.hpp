#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "model.hpp"   // zbog Mesh
#include <GL/glew.h>

class FloorLabels {
public:
    FloorLabels(const std::vector<std::string>& texturePaths);

    void Draw(
        Shader& shader,
        int numFloors,
        float floorHeight,
        glm::vec3 wallPos,
        glm::vec3 wallNormal,
        float size
    );

private:
    Mesh quad;
    std::vector<unsigned int> textures;

    glm::mat4 computeModelMatrix(
        int floorIndex,
        float floorHeight,
        glm::vec3 wallPos,
        glm::vec3 wallNormal,
        float size
    );
};
