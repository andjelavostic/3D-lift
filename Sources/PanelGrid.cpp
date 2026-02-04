#include "../Headers/PanelGrid.hpp"
#include "../Headers/model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

PanelGrid::PanelGrid(int r, int c)
    : rows(r), cols(c),
    panelMesh(
        {
            {{0, 0, 0}, {0, 0, 1}, {0, 0}},
            {{1, 0, 0}, {0, 0, 1}, {1, 0}},
            {{1, 1, 0}, {0, 0, 1}, {1, 1}},
            {{0, 1, 0}, {0, 0, 1}, {0, 1}}
        },
        { 0, 1, 2, 0, 2, 3 },
        {}
    )
{
    std::vector<std::string> paths = {
    "res/fan.png", "res/stop.png",
    "res/open.png", "res/close.png",
    "res/number-six.png", "res/number-five.png",
    "res/number-four.png", "res/number-3.png",
    "res/number-2.png", "res/number-one.png",
    "res/pr.png", "res/su.png"
    };
    for (auto& p : paths) {
        Texture t;
        t.id = TextureFromFile(p.c_str(), ".");
        t.type = "uDiffMap1";   // MORA DA se poklapa sa frag shaderom
        t.path = p;
        buttonTextures.push_back(t);
    }
}

void PanelGrid::attachToLiftWall(glm::vec3 pos, glm::vec3 wallNormal, float width, float height)
{
    normal = glm::normalize(wallNormal);

    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    if (fabs(normal.x) > 0.5f) {
        // rotacija da bude paralelno sa zidom
        model = glm::rotate(model,
            glm::radians(90.0f),
            glm::vec3(0, 1, 0));
    }

    model = glm::scale(model, glm::vec3(width, height, 1.0f));
}

void PanelGrid::Draw(Shader& shader)
{
    shader.use();
    shader.setInt("uDiffMap1", 0);

    float cellW = 1.0f / cols;
    float cellH = 1.0f / rows;
    float buttonScale = 0.75f;

    int i = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            glm::mat4 m = model;

            // ROTACIJA OKO X da dugmad gledaju ka kameri
            m = glm::rotate(m, glm::radians(180.0f), glm::vec3(1, 0, 0));

            // pozicija ćelije: levo→desno, odozgo→dole
            m = glm::translate(m,
                glm::vec3(c * cellW, (rows - 1 - r) * cellH, 0.001f));

            // centriranje dugmeta u ćeliji
            float offX = (cellW * (1.0f - buttonScale)) * 0.5f;
            float offY = (cellH * (1.0f - buttonScale)) * 0.5f;
            m = glm::translate(m, glm::vec3(offX, offY, 0));

            // skaliranje dugmeta
            m = glm::scale(m,
                glm::vec3(cellW * buttonScale, cellH * buttonScale, 1.0f));

            shader.setMat4("uM", m);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buttonTextures[i++].id);

            panelMesh.Draw(shader);
        }
    }
}
