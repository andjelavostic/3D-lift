#include "../Headers/FloorLabels.hpp"
#include <glm/gtc/matrix_transform.hpp>

FloorLabels::FloorLabels(const std::vector<std::string>& texturePaths)
    : quad(
        {
           {{0, 0, 0}, {0, 0, 1}, {0, 1}}, // donja leva → gornja leva
           {{1, 0, 0}, {0, 0, 1}, {1, 1}}, // donja desna → gornja desna
           {{1, 1, 0}, {0, 0, 1}, {1, 0}}, // gornja desna → donja desna
           {{0, 1, 0}, {0, 0, 1}, {0, 0}}  // gornja leva → donja leva
        },
        { 0, 1, 2, 0, 2, 3 },
        {}
    )
{
    for (const auto& path : texturePaths) {
        textures.push_back(TextureFromFile(path.c_str(), "."));
    }
}

glm::mat4 FloorLabels::computeModelMatrix(
    int floorIndex,
    float floorHeight,
    glm::vec3 wallPos,
    glm::vec3 wallNormal,
    float size
) {
    glm::mat4 m(1.0f);

    // pozicija po spratu
    m = glm::translate(m, wallPos + glm::vec3(0.0f, floorIndex * floorHeight, 0.0f));

    // orijentacija prema zidu
    if (wallNormal.x < -0.5f) {
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0, 1, 0));
    }
    else if (wallNormal.x > 0.5f) {
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    }
    else if (wallNormal.z < -0.5f) {
        m = glm::rotate(m, glm::radians(180.0f), glm::vec3(0, 1, 0));
    }
    m = glm::rotate(
        m,
        glm::radians(180.0f),
        glm::vec3(0, 1, 0)
    );
    // veličina nalepnice
    m = glm::scale(m, glm::vec3(size, size, 1.0f));

    return m;
}

void FloorLabels::Draw(
    Shader& shader,
    int numFloors,
    float floorHeight,
    glm::vec3 wallPos,
    glm::vec3 wallNormal,
    float size
) {
    shader.use();
    shader.setInt("uDiffMap1", 0);

    for (int i = 0; i < numFloors && i < textures.size(); i++) {

        glm::mat4 m = computeModelMatrix(
            i, floorHeight, wallPos, wallNormal, size
        );

        shader.setMat4("uM", m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        quad.Draw(shader);
    }
}
