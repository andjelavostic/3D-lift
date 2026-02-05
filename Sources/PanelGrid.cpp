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
    for (int i = 0; i < rows * cols; i++) {
        buttons.push_back(PanelButton{ i, false });
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
int PanelGrid::getButtonAtRay(glm::vec3 rayOrigin, glm::vec3 rayDir) {
    // invertuj model matricu da ray prebaciš u lokalni prostor panela
    glm::mat4 invM = glm::inverse(model);
    glm::vec3 localRayOrigin = glm::vec3(invM * glm::vec4(rayOrigin, 1.0f));
    glm::vec3 localRayDir = glm::normalize(glm::vec3(invM * glm::vec4(rayDir, 0.0f)));

    // provera paralelnosti sa panelom (z=0 u lokalnom prostoru)
    if (fabs(localRayDir.z) < 1e-6f) return -1;

    // nađi gde ray udara u panel
    float t = -localRayOrigin.z / localRayDir.z;
    if (t < 0) return -1; // iza kamere

    glm::vec3 hitPoint = localRayOrigin + t * localRayDir;

    // dugmad su raspoređena od 0 do 1 u X/Y
    float cellW = 1.0f / cols;
    float cellH = 1.0f / rows;

    int col = (int)(hitPoint.x / cellW);
    int row = (int)(hitPoint.y / cellH);

    // proveri da li je u granicama
    if (col < 0 || col >= cols || row < 0 || row >= rows) return -1;

    // raspored: gore→dole, levo→desno
    return (rows - 1 - row) * cols + col;
}

