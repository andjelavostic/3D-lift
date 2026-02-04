#include "../Headers/PanelGrid.hpp"
#include "../Headers/model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

PanelGrid::PanelGrid(int r, int c)
    : rows(r), cols(c),
    panelMesh(
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
    // Dugmad – REDOSLED = VIZUELNI REDOSLED
    std::vector<std::string> paths = {
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
        t.type = "uDiffMap1";
        t.path = p;
        buttonTextures.push_back(t);
    }
}
void PanelGrid::attachToLiftWall(
    glm::vec3 pos,
    glm::vec3 wallNormal,
    float width,
    float height)
{
    normal = glm::normalize(wallNormal);

    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    // LEVI ZID (X-)
    if (normal.x < -0.5f) {
        model = glm::rotate(
            model,
            glm::radians(90.0f),   // ⬅️ KLJUČNO: +90
            glm::vec3(0, 1, 0)
        );
    }
    // DESNI ZID (X+)
    else if (normal.x > 0.5f) {
        model = glm::rotate(
            model,
            glm::radians(-90.0f),
            glm::vec3(0, 1, 0)
        );
    }
    // ZADNJI ZID (Z-)
    else if (normal.z < -0.5f) {
        model = glm::rotate(
            model,
            glm::radians(180.0f),
            glm::vec3(0, 1, 0)
        );
    }
    // PREDNJI ZID (Z+)
    // nema rotacije – quad već gleda ka +Z

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

    // GORE → DOLE, LEVO → DESNO
    for (int r = rows - 1; r >= 0; r--) {
        for (int c = 0; c < cols; c++) {

            if (i >= buttonTextures.size())
                return;

            glm::mat4 m = model;

            // pozicija ćelije
            m = glm::translate(m, glm::vec3(
                c * cellW,
                r * cellH,
                0.002f
            ));

            // centriranje dugmeta
            float offX = (cellW * (1.0f - buttonScale)) * 0.5f;
            float offY = (cellH * (1.0f - buttonScale)) * 0.5f;
            m = glm::translate(m, glm::vec3(offX, offY, 0.0f));

            // skaliranje dugmeta
            m = glm::scale(m, glm::vec3(
                cellW * buttonScale,
                cellH * buttonScale,
                1.0f
            ));

            shader.setMat4("uM", m);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, buttonTextures[i].id);

            panelMesh.Draw(shader);
            i++;
        }
    }
}