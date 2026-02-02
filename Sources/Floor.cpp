#include "Floor.hpp"
#include <glm/gtc/matrix_transform.hpp>

Floor::Floor(const std::string& meshPath, glm::vec3 pos, float height)
    : floorMesh(meshPath), position(pos), floorHeight(height)
{
    // Mesh loader već kreira VAO/VBO, nema potrebe da ručno radiš
}

void Floor::draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);       // pozicija sprata
    shader.setMat4("uM", model);
    floorMesh.Draw(shader);                        // crta Mesh sa shaderom
}

void Floor::setPosition(const glm::vec3& pos) {
    position = pos;
}
