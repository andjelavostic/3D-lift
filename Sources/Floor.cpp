#include "../Headers/Floor.hpp"
#include <glm/gtc/matrix_transform.hpp>

Floor::Floor(const std::string& modelPath,
    glm::vec3 pos,
    float height)
    : floorModel(modelPath),
    position(pos),
    floorHeight(height)
{
}

void Floor::draw(Shader& shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.use();
    shader.setMat4("model", model);

    floorModel.Draw(shader);
}

void Floor::setPosition(const glm::vec3& pos)
{
    position = pos;
}
