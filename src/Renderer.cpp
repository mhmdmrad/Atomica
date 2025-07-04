#include "Renderer.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <glm/gtc/constants.hpp>

// ──────────────────────────────────────────────────────────────────────
// Sphere + line shader sources only

static const char* vertexSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;

out vec3 vNormal;
out vec3 vPos;

void main() {
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* fragSrc = R"(
#version 330 core
in vec3 vNormal;
in vec3 vPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - vPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = 0.1 * objectColor;
    vec3 diffuse = diff * objectColor;
    vec3 specular = spec * vec3(1.0);

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}
)";

static const char* lineVert = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

static const char* lineFrag = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 lineColor;

void main() {
    FragColor = vec4(lineColor, 1.0);
}
)";
// ──────────────────────────────────────────────────────────────────────

Renderer::Renderer(GLFWwindow* window)
    : m_window(window)
{
    glfwGetFramebufferSize(window, &m_windowWidth, &m_windowHeight);
    m_camera.setAspectRatio(float(m_windowWidth) / float(m_windowHeight));
}

Renderer::~Renderer() {
    if (m_lineVBO) glDeleteBuffers(1, &m_lineVBO);
    if (m_lineVAO) glDeleteVertexArrays(1, &m_lineVAO);
    if (m_sphereVBO) glDeleteBuffers(1, &m_sphereVBO);
    if (m_sphereEBO) glDeleteBuffers(1, &m_sphereEBO);
    if (m_sphereVAO) glDeleteVertexArrays(1, &m_sphereVAO);
}

bool Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    generateSphere(1.0f, 20, 20);
    glGenVertexArrays(1, &m_sphereVAO);
    glGenBuffers(1, &m_sphereVBO);
    glGenBuffers(1, &m_sphereEBO);

    glBindVertexArray(m_sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, m_sphereVertices.size() * sizeof(float), m_sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphereIndices.size() * sizeof(unsigned int), m_sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    if (!m_shaderManager.loadShader("sphere", vertexSrc, fragSrc)) return false;
    if (!m_shaderManager.loadShader("line", lineVert, lineFrag)) return false;

    std::cout << "Renderer initialized successfully\n";
    return true;
}

void Renderer::render(
    const std::vector<std::shared_ptr<Atom>>& atoms,
    const std::vector<std::shared_ptr<Molecule>>& molecules,
    float deltaTime)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_windowWidth, m_windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Debug object lifetimes
    std::cout << "Num Atoms: " << atoms.size()
              << " Num Molecules: " << molecules.size() << std::endl;

    for (auto& atom : atoms) {
        std::cout << "Atom Z: " << atom->getAtomicNumber()
                  << " Pos: " << atom->getPosition().x << ", "
                  << atom->getPosition().y << ", "
                  << atom->getPosition().z << std::endl;
        renderAtom(atom);
    }

    for (auto& mol : molecules) {
        for (auto& bond : mol->getBonds()) {
            auto A = bond->getAtom1();
            auto B = bond->getAtom2();
            std::cout << "Bond between: "
                      << A->getAtomicNumber() << " and "
                      << B->getAtomicNumber() << std::endl;
            renderBond(bond);
        }
    }

    renderEnergyLabels(deltaTime);
}

void Renderer::onWindowResize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    glViewport(0, 0, width, height);
    m_camera.setAspectRatio(float(width) / float(height));
}

void Renderer::addEnergyLabel(const glm::vec3& position, float energy, float duration) {
    EnergyLabel lbl;
    lbl.position = position;
    lbl.energy = energy;
    lbl.remainingTime = duration;
    lbl.fontSize = std::clamp(std::abs(energy) / 10.0f, 0.5f, 2.0f);
    m_energyLabels.push_back(lbl);
}

glm::vec3 Renderer::getAtomColor(int Z) const {
    switch (Z) {
    case 1:  return {1, 1, 1};
    case 6:  return {0.2f, 0.2f, 0.2f};
    case 7:  return {0, 0, 1};
    case 8:  return {1, 0, 0};
    case 15: return {1, 0.5f, 0};
    case 16: return {1, 1, 0};
    default: return {0.5f, 0.5f, 0.5f};
    }
}

float Renderer::getAtomRadius(int Z) const {
    switch (Z) {
    case 1:  return 0.3f; // Hydrogen
    case 6:  return 0.5f; // Carbon
    case 7:  return 0.45f; // Nitrogen
    case 8:  return 0.4f; // Oxygen
    case 15: return 0.55f; // Phosphorus
    case 16: return 0.6f; // Sulfur
    default: return 0.5f;
    }
}

void Renderer::generateSphere(float radius, int sectorCount, int stackCount) {
    m_sphereVertices.clear();
    m_sphereIndices.clear();
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::half_pi<float>() - i * glm::pi<float>() / stackCount;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);
        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            m_sphereVertices.insert(m_sphereVertices.end(), { x, y, z, x / radius, y / radius, z / radius });
        }
    }
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                m_sphereIndices.push_back(k1);
                m_sphereIndices.push_back(k2);
                m_sphereIndices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                m_sphereIndices.push_back(k1 + 1);
                m_sphereIndices.push_back(k2);
                m_sphereIndices.push_back(k2 + 1);
            }
        }
    }
}

void Renderer::renderAtom(std::shared_ptr<Atom> atom) { /* … */ }
void Renderer::renderBond(std::shared_ptr<Bond> bond) {
    m_shaderManager.useShader("line");
    float pts[6] = {
       bond->getAtom1()->getPosition().x, /*…*/ bond->getAtom2()->getPosition().z
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_DYNAMIC_DRAW);
    m_shaderManager.setUniformMat4("view",       m_camera.getViewMatrix());
    m_shaderManager.setUniformMat4("projection", m_camera.getProjectionMatrix());
    m_shaderManager.setUniformVec3("lineColor",  glm::vec3(0.8f));
    glBindVertexArray(m_lineVAO);
      glLineWidth(3.0f);
      glDrawArrays(GL_LINES, 0, 2);
      glLineWidth(1.0f);
    glBindVertexArray(0);
}

void Renderer::renderEnergyLabels(float deltaTime) {
    for (auto it = m_energyLabels.begin(); it!=m_energyLabels.end();) {
        it->remainingTime -= dt;
        if (it->remainingTime <= 0) it = m_energyLabels.erase(it);
        else ++it;
    }
}

// ——— Photon code ———

void Renderer::triggerPhotonDisplay(float wavelengthNm,
                                    Band band,
                                    const glm::vec3& origin)
{
    m_showPhoton       = true;
    m_photonWavelength = wavelengthNm;
    m_photonBand       = band;
    m_photonOrigin     = origin;
    m_photonFramesLeft = PHOTON_FADE_FRAMES;
    m_photonAlpha      = 1.0f;
}

glm::vec3 Renderer::wavelengthToRGB(float λ) const {
    if (λ < 380 || λ > 750) return {1,1,1};
    float t;
    if (λ < 440)   { t=(λ-380)/60;    return {1-t,0,1}; }
    if (λ < 490)   { t=(λ-440)/50;    return {0,t,1}; }
    if (λ < 510)   { t=(λ-490)/20;    return {0,1,1-t}; }
    if (λ < 580)   { t=(λ-510)/70;    return {t,1,0}; }
    if (λ < 645)   { t=(λ-580)/65;    return {1,1-t,0}; }
    /* λ ≤750 */   { t=(λ-645)/105;   return {1,0,t}; }
}

void Renderer::displayPhoton() {
    if (!m_showPhoton || m_photonFramesLeft<=0) return;

    glm::vec3 col;
    switch (m_photonBand) {
      case Band::VISIBLE:     col = wavelengthToRGB(m_photonWavelength); break;
      case Band::ULTRAVIOLET: col = {0.6f,0,0.8f}; break;
      case Band::INFRARED:    col = {1.0f,0.3f,0}; break;
    }
    col *= m_photonAlpha;

    const int N = 50; const float length=2.0f;
    std::vector<glm::vec3> pts(N);
    for(int i=0;i<N;++i){
        float t = float(i)/(N-1);
        float x = m_photonOrigin.x + t*length;
        float y = 0.2f * std::sin(2.0f*glm::pi<float>()*t * (750.0f/m_photonWavelength));
        pts[i] = { x, m_photonOrigin.y + y, m_photonOrigin.z };
    }

    m_shaderManager.useShader("line");
    m_shaderManager.setUniformMat4("view",       m_camera.getViewMatrix());
    m_shaderManager.setUniformMat4("projection", m_camera.getProjectionMatrix());
    m_shaderManager.setUniformVec3("lineColor",  col);

    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*pts.size(), pts.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(m_lineVAO);
      glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)pts.size());
    glBindVertexArray(0);

    // fade
    --m_photonFramesLeft;
    m_photonAlpha = float(m_photonFramesLeft)/PHOTON_FADE_FRAMES;
    if (m_photonFramesLeft<=0) m_showPhoton = false;
}