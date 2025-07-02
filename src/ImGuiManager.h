#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "Atom.h"
#include "Molecule.h"
#include "PhysicsEngine.h"

class ImGuiManager {
public:
    ImGuiManager(GLFWwindow* window);
    ~ImGuiManager();

    bool initialize();
    void newFrame();
    void render(PhysicsEngine& physicsEngine);
    void endFrame();
    bool isMouseOverUI() const;

private:
    GLFWwindow* m_window;

    // UI state
    int   m_selectedAtomicNumber   = 1;
    int   m_selectedMassNumber     = 1;
    bool  m_bondingMode            = false;
    bool  m_fissionMode            = false;
    bool  m_fusionMode             = false;
    int   m_selectedElectronOrbital= 1;
    int   m_targetOrbital          = 2;

    std::shared_ptr<Atom>    m_selectedAtom1;
    std::shared_ptr<Atom>    m_selectedAtom2;

    void renderAtomPalette(PhysicsEngine& physicsEngine);
    void renderBondingControls(PhysicsEngine& physicsEngine);
    void renderNuclearControls(PhysicsEngine& physicsEngine);
    void renderOrbitalControls(PhysicsEngine& physicsEngine);
    void renderSimulationInfo(PhysicsEngine& physicsEngine);

    std::string getElementName(int atomicNumber) const;
};
