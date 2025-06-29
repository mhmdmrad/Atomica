#include "Renderer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <glm/gtc/constants.hpp>

// ──────────────────────────────────────────────────────────────────────
// Shader stuff:
// FXAA shader sources
static const char* fxaaVertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
out vec2 vUV;
void main() {
  vUV = aUV;
  gl_Position = vec4(aPos, 0.0, 1.0);
})";

static const char* fxaaFragSrc = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D screenTexture;
uniform vec2 resolution;
void main() {
  // simple pass‐through FXAA placeholder:
  FragColor = texture(screenTexture, vUV);
})";

static const char* vertexSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3  objectColor;

out vec3 vNormal;
out vec3 vPos;

void main() {
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vPos    = vec3(model * vec4(aPos, 1.0));
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
    // simple Phong lighting
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
    // Get initial viewport size
    glfwGetFramebufferSize(window, &m_windowWidth, &m_windowHeight);
    m_camera.setAspectRatio(
        float(m_windowWidth) / float(m_windowHeight)
    );
}

Renderer::~Renderer() {
    // Clean up all GL objects (quad, FBO, line, sphere)
    if (m_quadVBO)     glDeleteBuffers(1, &m_quadVBO);
    if (m_quadVAO)     glDeleteVertexArrays(1, &m_quadVAO);

    if (m_sceneDepthRBO) glDeleteRenderbuffers(1, &m_sceneDepthRBO);
    if (m_sceneColorTex) glDeleteTextures(1,    &m_sceneColorTex);
    if (m_sceneFBO)      glDeleteFramebuffers(1, &m_sceneFBO);

    if (m_lineVBO)     glDeleteBuffers(1, &m_lineVBO);
    if (m_lineVAO)     glDeleteVertexArrays(1, &m_lineVAO);

    if (m_sphereEBO)   glDeleteBuffers(1, &m_sphereEBO);
    if (m_sphereVBO)   glDeleteBuffers(1, &m_sphereVBO);
    if (m_sphereVAO)   glDeleteVertexArrays(1, &m_sphereVAO);
}

bool Renderer::initialize() {
    // 1) Core GL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2) Prepare sphere geometry
    generateSphere(1.0f, 20, 20);
    glGenVertexArrays(1, &m_sphereVAO);
    glGenBuffers(1, &m_sphereVBO);
    glGenBuffers(1, &m_sphereEBO);

    glBindVertexArray(m_sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizei)(m_sphereVertices.size() * sizeof(float)),
                     m_sphereVertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizei)(m_sphereIndices.size() * sizeof(unsigned int)),
                     m_sphereIndices.data(),
                     GL_STATIC_DRAW);
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 3) Prepare line geometry
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    glBindVertexArray(m_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // 4) Compile & link shaders
    if (!m_shaderManager.loadShader("sphere", vertexSrc, fragSrc)) return false;
    if (!m_shaderManager.loadShader("line",   lineVert,  lineFrag)) return false;
    if (!m_shaderManager.loadShader("fxaa",   fxaaVertSrc, fxaaFragSrc)) {
        std::cerr << "Failed to load FXAA shader\n";
        return false;
    }

    // 5) Offscreen FBO + attachments
    glGenFramebuffers(1, &m_sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);

        // 5.1) Color texture
        glGenTextures(1, &m_sceneColorTex);
        glBindTexture(GL_TEXTURE_2D, m_sceneColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     m_windowWidth, m_windowHeight, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_sceneColorTex, 0);

        // 5.2) Depth‑stencil renderbuffer
        glGenRenderbuffers(1, &m_sceneDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH24_STENCIL8,
                              m_windowWidth, m_windowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  m_sceneDepthRBO);

        // 5.3) Check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "FXAA FBO incomplete\n";
            return false;
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 6) Fullscreen quad setup
    static const float quadVerts[] = {
        -1,  1,  0, 1,
        -1, -1,  0, 0,
         1, -1,  1, 0,
        -1,  1,  0, 1,
         1, -1,  1, 0,
         1,  1,  1, 1
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1,      &m_quadVBO);
    glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    std::cout << "Renderer initialized successfully\n";
    return true;
}

void Renderer::render(
    const std::vector<std::shared_ptr<Atom>>& atoms,
    const std::vector<std::shared_ptr<Molecule>>& molecules,
    float deltaTime)
{
    // 1) Render scene into offscreen FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_sceneFBO);
    glViewport(0, 0, m_windowWidth, m_windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& atom : atoms)
        renderAtom(atom);

    for (auto& mol : molecules)
        for (auto& bond : mol->getBonds())
            renderBond(bond);

    renderEnergyLabels(deltaTime);

    // 2) FXAA pass back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shaderManager.useShader("fxaa");
    m_shaderManager.setUniformInt("screenTexture", 0);
    m_shaderManager.setUniformVec2(
        "resolution",
        glm::vec2(float(m_windowWidth), float(m_windowHeight))
    );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sceneColorTex);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::onWindowResize(int width, int height) {
    m_windowWidth  = width;
    m_windowHeight = height;
    glViewport(0,0,width,height);
    m_camera.setAspectRatio(float(width)/float(height));
}

void Renderer::addEnergyLabel(const glm::vec3& position,
                             float energy,
                             float duration)
{
    EnergyLabel lbl;
    lbl.position      = position;
    lbl.energy        = energy;
    lbl.remainingTime = duration;
    lbl.fontSize      = std::clamp(std::abs(energy)/10.0f, 0.5f, 2.0f);
    m_energyLabels.push_back(lbl);
}

glm::vec3 Renderer::getAtomColor(int Z) const {
    switch(Z) {
      case 1:  return {1,1,1};      // H
      case 6:  return {0.2f,0.2f,0.2f}; // C
      case 7:  return {0,0,1};      // N
      case 8:  return {1,0,0};      // O
      case 15: return {1,0.5f,0};   // P
      case 16: return {1,1,0};      // S
      default: return {0.5f,0.5f,0.5f};
    }
}

float Renderer::getAtomRadius(int Z) const {
    switch(Z) {
      case 1:  return 0.3f;
      case 6:  return 0.5f;
      case 7:  return 0.45f;
      case 8:  return 0.4f;
      case 15: return 0.55f;
      case 16: return 0.6f;
      default: return 0.5f;
    }
}

void Renderer::generateSphere(float radius,
                              int sectorCount,
                              int stackCount)
{
    m_sphereVertices.clear();
    m_sphereIndices.clear();

    for(int i=0; i<=stackCount; ++i){
        float stackAngle = glm::half_pi<float>()
                         - i * glm::pi<float>()/stackCount;
        float xy = radius*cos(stackAngle);
        float z  = radius*sin(stackAngle);

        for(int j=0; j<=sectorCount; ++j){
            float sectorAngle = j*2*glm::pi<float>()
                               /sectorCount;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            m_sphereVertices.insert(
              m_sphereVertices.end(),
              {x,y,z,
               x/radius, y/radius, z/radius}
            );
        }
    }

    for(int i=0; i<stackCount; ++i){
        int k1 = i*(sectorCount+1);
        int k2 = k1 + sectorCount + 1;
        for(int j=0; j<sectorCount; ++j, ++k1, ++k2){
            if(i!=0){
                m_sphereIndices.push_back(k1);
                m_sphereIndices.push_back(k2);
                m_sphereIndices.push_back(k1+1);
            }
            if(i!=(stackCount-1)){
                m_sphereIndices.push_back(k1+1);
                m_sphereIndices.push_back(k2);
                m_sphereIndices.push_back(k2+1);
            }
        }
    }
}

void Renderer::renderAtom(std::shared_ptr<Atom> atom) {
    m_shaderManager.useShader("sphere");

    // model matrix for this atom
    glm::mat4 model = glm::translate(
        glm::mat4(1.0f), atom->getPosition()
    );
    float r = getAtomRadius(atom->getAtomicNumber());
    model = glm::scale(model, glm::vec3(r));

    // set uniforms
    m_shaderManager.setUniformMat4("model",      model);
    m_shaderManager.setUniformMat4("view",       m_camera.getViewMatrix());
    m_shaderManager.setUniformMat4("projection", m_camera.getProjectionMatrix());
    m_shaderManager.setUniformVec3("objectColor",
                                  getAtomColor(atom->getAtomicNumber()));
    m_shaderManager.setUniformVec3("lightPos",
                                  glm::vec3(5,5,5));
    m_shaderManager.setUniformVec3("viewPos",
                                  m_camera.getPosition());

    glBindVertexArray(m_sphereVAO);
    glDrawElements(GL_TRIANGLES,
                   (GLsizei)m_sphereIndices.size(),
                   GL_UNSIGNED_INT,
                   0);
    glBindVertexArray(0);
}

void Renderer::renderBond(std::shared_ptr<Bond> bond) {
    m_shaderManager.useShader("line");

    // use actual API: getAtom1/getAtom2
    auto A = bond->getAtom1();
    auto B = bond->getAtom2();

    float pts[6] = {
      A->getPosition().x, A->getPosition().y, A->getPosition().z,
      B->getPosition().x, B->getPosition().y, B->getPosition().z
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_DYNAMIC_DRAW);

    // uniforms
    m_shaderManager.setUniformMat4("view",       m_camera.getViewMatrix());
    m_shaderManager.setUniformMat4("projection", m_camera.getProjectionMatrix());
    m_shaderManager.setUniformVec3("lineColor",  glm::vec3(0.8f,0.8f,0.8f));

    glBindVertexArray(m_lineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::renderEnergyLabels(float deltaTime) {
    for (auto it = m_energyLabels.begin(); it!=m_energyLabels.end();) {
        it->remainingTime -= deltaTime;
        if (it->remainingTime <= 0.0f) {
            it = m_energyLabels.erase(it);
        } else {
            // TODO: your text‐drawing code here, using it->position, it->fontSize
            ++it;
        }
    }
}
