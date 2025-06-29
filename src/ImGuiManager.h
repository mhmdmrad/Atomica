#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "Atom.h"
#include "Molecule.h"
#include "PhysicsEngine.h"

/**
 * @brief Manages ImGui interface for the simulation.
 * 
 * This class handles all UI panels including atom palette,
 * bonding controls, nuclear event controls, and orbital controls.
 */
class ImGuiManager {
public:
    /**
     * @brief Constructs a new ImGuiManager object.
     * 
     * @param window The GLFW window handle.
     */
    ImGuiManager(GLFWwindow* window);

    /**
     * @brief Destroys the ImGuiManager object.
     */
    ~ImGuiManager();

    /**
     * @brief Initializes ImGui.
     * 
     * @return True if initialization was successful, false otherwise.
     */
    bool initialize();

    /**
     * @brief Begins a new ImGui frame.
     */
    void newFrame();

    /**
     * @brief Renders the UI panels.
     * 
     * @param physicsEngine Reference to the physics engine for interaction.
     */
    void render(PhysicsEngine& physicsEngine);

    /**
     * @brief Ends the ImGui frame and renders it.
     */
    void endFrame();

    /**
     * @brief Checks if the mouse is over any ImGui window.
     * 
     * @return True if mouse is over ImGui, false otherwise.
     */
    bool isMouseOverUI() const;

private:
    GLFWwindow* m_window;
    
    // UI state variables
    int m_selectedAtomicNumber;
    int m_selectedMassNumber;
    bool m_bondingMode;
    std::shared_ptr<Atom> m_selectedAtom1;
    std::shared_ptr<Atom> m_selectedAtom2;
    
    // Nuclear event controls
    bool m_fissionMode;
    bool m_fusionMode;
    
    // Orbital controls
    int m_selectedElectronOrbital;
    int m_targetOrbital;

    /**
     * @brief Renders the atom palette panel.
     * 
     * @param physicsEngine Reference to the physics engine.
     */
    void renderAtomPalette(PhysicsEngine& physicsEngine);

    /**
     * @brief Renders the bonding controls panel.
     * 
     * @param physicsEngine Reference to the physics engine.
     */
    void renderBondingControls(PhysicsEngine& physicsEngine);

    /**
     * @brief Renders the nuclear event controls panel.
     * 
     * @param physicsEngine Reference to the physics engine.
     */
    void renderNuclearControls(PhysicsEngine& physicsEngine);

    /**
     * @brief Renders the orbital controls panel.
     * 
     * @param physicsEngine Reference to the physics engine.
     */
    void renderOrbitalControls(PhysicsEngine& physicsEngine);

    /**
     * @brief Renders the simulation info panel.
     * 
     * @param physicsEngine Reference to the physics engine.
     */
    void renderSimulationInfo(PhysicsEngine& physicsEngine);

    /**
     * @brief Gets the name of an element from its atomic number.
     * 
     * @param atomicNumber The atomic number.
     * @return The element name.
     */
    std::string getElementName(int atomicNumber) const;
};

#endif // IMGUI_MANAGER_H

