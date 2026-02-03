#include "../Headers/PanelGrid.hpp"
#include "../Headers/model.hpp"
#include <cmath>

PanelGrid::PanelGrid(int r, int c)
    : rows(r), cols(c),
    panelMesh(
        // vertices
        std::vector<Vertex>{
            {{0, 0, 0}, { 0, 0, 1 }, { 0, 0 }},
            { {1, 0, 0}, {0, 0, 1}, {1, 0} },
            { {1, 1, 0}, {0, 0, 1}, {1, 1} },
            { {0, 1, 0}, {0, 0, 1}, {0, 1} }
},
// indices
std::vector<unsigned int>{0, 1, 2, 0, 2, 3},
// empty textures
std::vector<Texture>{}
)
{

    // ---------- textures ----------
    vector<string> paths = {
        "res/su.png", "res/pr.png",
        "res/number-one.png", "res/number-2.png",
        "res/number-3.png", "res/number-four.png",
        "res/number-five.png", "res/number-six.png",
        "res/close.png", "res/open.png",
        "res/stop.png", "res/fan.png"
    };

    for (auto& p : paths) {
        Texture t;
        t.id = TextureFromFile(p.c_str(), ".");
        t.type = "uDiffMap";
        t.path = p;
        buttonTextures.push_back(t);
    }
}
void PanelGrid::attachToLiftWall(glm::vec3 pos,
    glm::vec3 wallNormal,
    float width,
    float height)
{
    normal = glm::normalize(wallNormal);

    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    if (fabs(normal.x) > 0.5f) {
        model = glm::rotate(model,
            glm::radians(90.0f),
            glm::vec3(0, 1, 0));
    }

    model = glm::scale(model,
        glm::vec3(width, height, 1.0f));
}
void PanelGrid::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", model);

    int i = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            shader.setVec4("uvRect",
                float(c) / cols,
                float(rows - r - 1) / rows,
                float(c + 1) / cols,
                float(rows - r) / rows
            );

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buttonTextures[i++].id);

            panelMesh.Draw(shader);
        }
    }
}
int PanelGrid::checkClick(glm::vec3 rayOrigin,
    glm::vec3 rayDir,
    bool inLift)
{
    if (!inLift) return -1;

    float denom = glm::dot(normal, rayDir);
    if (fabs(denom) < 0.0001f) return -1;

    glm::vec3 pos = glm::vec3(model[3]);
    float t = glm::dot(pos - rayOrigin, normal) / denom;
    if (t < 0) return -1;

    glm::vec3 hit = rayOrigin + rayDir * t;
    glm::vec3 local =
        glm::inverse(model) * glm::vec4(hit, 1.0f);

    if (local.x < 0 || local.x > 1 ||
        local.y < 0 || local.y > 1)
        return -1;

    int col = int(local.x * cols);
    int row = int((1.0f - local.y) * rows);

    return row * cols + col;
}

