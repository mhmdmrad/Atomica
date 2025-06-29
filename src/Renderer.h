#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>

#include "Camera.h"
#include "ShaderManager.h"
#include "Atom.h"
#include "Molecule.h"
#include "Bond.h"

/**
 * @brief Handles all OpenGL rendering operations for the simulation.
 */
class Renderer {
public:
    Renderer(GLFWwindow* window);
    ~Renderer();

    bool initialize();

    void render(
        const std::vector<std::shared_ptr<Atom>>& atoms,
        const std::vector<std::shared_ptr<Molecule>>& molecules,
        float deltaTime
    );

    Camera& getCamera() { return m_camera; }
    void    onWindowResize(int width, int height);
    void    addEnergyLabel(const glm::vec3& position, float energy, float duration = 3.0f);

private:
    struct EnergyLabel {
        glm::vec3 position;
        float     energy;
        float     remainingTime;
        float     fontSize;
    };

    GLFWwindow*                   m_window;
    Camera                        m_camera;
    ShaderManager                 m_shaderManager;
    std::vector<float>            m_sphereVertices;
    std::vector<unsigned int>     m_sphereIndices;

    // Sphere geometry
    GLuint m_sphereVAO = 0,
           m_sphereVBO = 0,
           m_sphereEBO = 0;

    // Line geometry
    GLuint m_lineVAO = 0,
           m_lineVBO = 0;

    // Offscreen FBO + attachments for FXAA
    GLuint m_sceneFBO      = 0;
    GLuint m_sceneColorTex = 0;
    GLuint m_sceneDepthRBO = 0;

    // Screen‐quad
    GLuint m_quadVAO = 0,
           m_quadVBO = 0;

    std::vector<EnergyLabel> m_energyLabels;
    int m_windowWidth  = 800;
    int m_windowHeight = 600;

    // Internal helpers
    void generateSphere(float radius, int sectorCount, int stackCount);
    void renderAtom(std::shared_ptr<Atom> atom);
    void renderBond(std::shared_ptr<Bond> bond);
    void renderEnergyLabels(float deltaTime);
    glm::vec3 getAtomColor(int atomicNumber) const;
    float     getAtomRadius(int atomicNumber) const;
};
