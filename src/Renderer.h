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

    // Photon‐wave display API
    enum class Band { ULTRAVIOLET, VISIBLE, INFRARED };
    static constexpr int PHOTON_FADE_FRAMES = 60;

    /// Trigger a photon‐wave at origin, fading out over PHOTON_FADE_FRAMES
    void triggerPhotonDisplay(float wavelengthNm,
                              Band band,
                              const glm::vec3& origin);

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

    std::vector<EnergyLabel>      m_energyLabels;
    int                           m_windowWidth  = 800;
    int                           m_windowHeight = 600;

    // Photon state
    bool      m_showPhoton       = false;
    float     m_photonWavelength = 0.0f;
    Band      m_photonBand       = Band::VISIBLE;
    glm::vec3 m_photonOrigin     = glm::vec3(0.0f);
    int       m_photonFramesLeft = 0;
    float     m_photonAlpha      = 0.0f;

    // Internal helpers
    void generateSphere(float radius, int sectorCount, int stackCount);
    void renderAtom(std::shared_ptr<Atom> atom);
    void renderBond(std::shared_ptr<Bond> bond);
    void renderEnergyLabels(float deltaTime);
    glm::vec3 getAtomColor(int atomicNumber) const;
    float     getAtomRadius(int atomicNumber) const;

    // Photon helpers
    void      displayPhoton();
    glm::vec3 wavelengthToRGB(float wavelength) const;
};