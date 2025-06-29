#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

// OpenGL and windowing
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Physics engine
#include "PhysicsEngine.h"
#include "Atom.h"
#include "Molecule.h"
#include "Bond.h"
#include "BondCalculator.h"
#include "NuclearReactor.h"
#include "OrbitalModel.h"

// Rendering
#include "Renderer.h"
#include "ImGuiManager.h"

// Utilities
#include "Logger.h"
#include "ConfigManager.h"
#include "MathUtils.h"

/**
 * @brief Main application class for the Sandbox Simulation.
 * 
 * This class manages the main application loop, window creation,
 * and coordinates between the physics engine and renderer.
 */

class SandboxSimulation {
public:
    /**
     * @brief Constructs a new SandboxSimulation object.
     */
    SandboxSimulation();

    /**
     * @brief Destroys the SandboxSimulation object.
     */
    ~SandboxSimulation();

    /**
     * @brief Initializes the application.
     * 
     * @return True if initialization was successful.
     */
    bool initialize();

    /**
     * @brief Runs the main application loop.
     */
    void run();

private:
    // Window and rendering
    GLFWwindow* m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<ImGuiManager> m_imguiManager;
    
    // Physics
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    
    // Application state
    bool m_running;
    int m_windowWidth;
    int m_windowHeight;
    
    // Mouse state
    bool m_firstMouse;
    float m_lastX, m_lastY;

    /**
     * @brief Initializes GLFW and creates the window.
     * 
     * @return True if successful.
     */
    bool initializeWindow();

    /**
     * @brief Initializes OpenGL context.
     * 
     * @return True if successful.
     */
    bool initializeOpenGL();

    /**
     * @brief Sets up the initial simulation scene.
     */
    void setupScene();

    /**
     * @brief Demonstrates H2O molecule creation and bond manipulation.
     */
    void demonstrateH2OMolecule();

    /**
     * @brief Demonstrates nuclear fission event.
     */
    void demonstrateFission();

    /**
     * @brief Demonstrates electron orbital transition.
     */
    void demonstrateElectronJump();

    /**
     * @brief Updates the simulation for one frame.
     * 
     * @param deltaTime Time elapsed since last frame.
     */
    void update(float deltaTime);

    /**
     * @brief Renders one frame.
     * 
     * @param deltaTime Time elapsed since last frame.
     */
    void render(float deltaTime);

    /**
     * @brief Handles input events.
     */
    void handleInput();

    /**
     * @brief Cleanup resources.
     */
    void cleanup();

    // GLFW callback functions
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

SandboxSimulation::SandboxSimulation()
    : m_window(nullptr),
      m_running(false),
      m_windowWidth(1200),
      m_windowHeight(800),
      m_firstMouse(true),
      m_lastX(0.0f),
      m_lastY(0.0f) {}

SandboxSimulation::~SandboxSimulation() {
    cleanup();
}

bool SandboxSimulation::initialize() {
    // Initialize logger
    Logger::getInstance().setLogLevel(Logger::Level::INFO);
    Logger::getInstance().setLogFile("simulation.log");
    LOG_INFO("Starting Sandbox Simulation");

    // Load configuration
    ConfigManager::getInstance().loadFromFile("config.ini");
    m_windowWidth = ConfigManager::getInstance().getInt("window_width", 1200);
    m_windowHeight = ConfigManager::getInstance().getInt("window_height", 800);

    // Initialize window and OpenGL
    if (!initializeWindow()) {
        LOG_ERROR("Failed to initialize window");
        return false;
    }

    if (!initializeOpenGL()) {
        LOG_ERROR("Failed to initialize OpenGL");
        return false;
    }

    // Initialize renderer
    m_renderer = std::make_unique<Renderer>(m_window);
    if (!m_renderer->initialize()) {
        LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    // Initialize ImGui
    m_imguiManager = std::make_unique<ImGuiManager>(m_window);
    if (!m_imguiManager->initialize()) {
        LOG_ERROR("Failed to initialize ImGui");
        return false;
    }

    // Initialize physics engine
    m_physicsEngine = std::make_unique<PhysicsEngine>();

    // Setup initial scene
    setupScene();

    m_running = true;
    LOG_INFO("Sandbox Simulation initialized successfully");
    return true;
}

void SandboxSimulation::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (m_running && !glfwWindowShouldClose(m_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Handle input
        handleInput();

        // Update simulation
        update(deltaTime);

        // Render
        render(deltaTime);

        // Swap buffers and poll events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

bool SandboxSimulation::initializeWindow() {
    // Initialize GLFW
    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Sandbox Simulation", nullptr, nullptr);
    if (!m_window) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Set callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);

    // Configure cursor
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return true;
}

bool SandboxSimulation::initializeOpenGL() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        // single-argument LOG_ERROR
        LOG_ERROR(std::string("Failed to initialize GLEW: ")
                  + reinterpret_cast<const char*>(glewGetErrorString(err)));
        return false;
    }
    return true;
}

void SandboxSimulation::setupScene() {
    LOG_INFO("Setting up initial scene");
    
    // Demonstrate H2O molecule creation
    demonstrateH2OMolecule();
    
    // Add a few more atoms for demonstration
    auto carbon = std::make_shared<Atom>(6, 12, glm::vec3(3.0f, 0.0f, 0.0f));
    auto nitrogen = std::make_shared<Atom>(7, 14, glm::vec3(-3.0f, 0.0f, 0.0f));
    
    m_physicsEngine->addAtom(carbon);
    m_physicsEngine->addAtom(nitrogen);
    
    LOG_INFO("Initial scene setup complete");
}

void SandboxSimulation::demonstrateH2OMolecule() {
    LOG_INFO("Creating H2O molecule demonstration");
    
    // Create atoms: 1 Oxygen + 2 Hydrogen
    auto oxygen = std::make_shared<Atom>(8, 16, glm::vec3(0.0f, 0.0f, 0.0f));
    auto hydrogen1 = std::make_shared<Atom>(1, 1, glm::vec3(1.0f, 0.5f, 0.0f));
    auto hydrogen2 = std::make_shared<Atom>(1, 1, glm::vec3(-1.0f, 0.5f, 0.0f));
    
    // Create molecule
    auto h2o = std::make_shared<Molecule>();
    h2o->addAtom(oxygen);
    h2o->addAtom(hydrogen1);
    h2o->addAtom(hydrogen2);
    
    // Create bonds
    BondCalculator bondCalc;
    
    auto bond1 = std::make_shared<Bond>(oxygen, hydrogen1, 
                                       bondCalc.determineBondType(oxygen, hydrogen1),
                                       bondCalc.getBondEnergy(bondCalc.determineBondType(oxygen, hydrogen1)));
    
    auto bond2 = std::make_shared<Bond>(oxygen, hydrogen2,
                                       bondCalc.determineBondType(oxygen, hydrogen2),
                                       bondCalc.getBondEnergy(bondCalc.determineBondType(oxygen, hydrogen2)));
    
    h2o->addBond(bond1);
    h2o->addBond(bond2);
    
    // Add to physics engine
    m_physicsEngine->addMolecule(h2o);
    
    // Add energy labels for bond formation
    m_renderer->addEnergyLabel(glm::vec3(0.5f, 0.25f, 0.0f), bond1->getEnergy(), 5.0f);
    m_renderer->addEnergyLabel(glm::vec3(-0.5f, 0.25f, 0.0f), bond2->getEnergy(), 5.0f);
    
    LOG_INFO("H2O molecule created with bond energies: " + 
             std::to_string(bond1->getEnergy()) + " eV and " + 
             std::to_string(bond2->getEnergy()) + " eV");
}

void SandboxSimulation::demonstrateFission() {
    LOG_INFO("Demonstrating nuclear fission");
    
    // Create U-235 nucleus for fission
    auto uranium = std::make_shared<Atom>(92, 235, glm::vec3(5.0f, 0.0f, 0.0f));
    m_physicsEngine->addAtom(uranium);
    
    // Simulate fission
    NuclearReactor reactor;
    float fissionEnergy = reactor.simulateFission(uranium->getNucleus());
    
    // Add energy label
    m_renderer->addEnergyLabel(glm::vec3(5.0f, 1.0f, 0.0f), fissionEnergy, 10.0f);
    
    LOG_INFO("Fission demonstration: Released " + std::to_string(fissionEnergy) + " eV");
}

void SandboxSimulation::demonstrateElectronJump() {
    LOG_INFO("Demonstrating electron orbital transition");
    
    // Get first atom for electron jump demonstration
    const auto& atoms = m_physicsEngine->getAtoms();
    if (!atoms.empty()) {
        auto atom = atoms[0];
        const auto& electrons = atom->getElectrons();
        
        if (!electrons.empty()) {
            OrbitalModel orbitalModel;
            float deltaE = orbitalModel.simulateElectronJump(electrons[0], atom, 3);
            
            // Add energy label
            m_renderer->addEnergyLabel(atom->getPosition() + glm::vec3(0.0f, 1.5f, 0.0f), 
                                     deltaE, 8.0f);
            
            LOG_INFO("Electron jump demonstration: ΔE = " + std::to_string(deltaE) + " eV");
        }
    }
}

void SandboxSimulation::update(float deltaTime) {
    // Update physics
    m_physicsEngine->update(deltaTime);
    
    // Trigger demonstrations periodically (for example purposes)
    static float demoTimer = 0.0f;
    demoTimer += deltaTime;
    
    if (demoTimer > 10.0f) { // Every 10 seconds
        // Demonstrate fission
        demonstrateFission();
        
        // Demonstrate electron jump
        demonstrateElectronJump();
        
        demoTimer = 0.0f;
    }
}

void SandboxSimulation::render(float deltaTime) {
    // Start ImGui frame
    m_imguiManager->newFrame();
    
    // Render 3D scene
    m_renderer->render(m_physicsEngine->getAtoms(), m_physicsEngine->getMolecules(), deltaTime);
    
    // Render UI
    m_imguiManager->render(*m_physicsEngine);
    
    // End ImGui frame
    m_imguiManager->endFrame();
}

void SandboxSimulation::handleInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_running = false;
    }
}

void SandboxSimulation::cleanup() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
    
    LOG_INFO("Sandbox Simulation shutdown complete");
}

// GLFW Callbacks
void SandboxSimulation::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    app->m_windowWidth = width;
    app->m_windowHeight = height;
    app->m_renderer->onWindowResize(width, height);
}

void SandboxSimulation::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    
    if (app->m_imguiManager->isMouseOverUI()) {
        return; // Don't process camera movement if mouse is over UI
    }
    
    if (app->m_firstMouse) {
        app->m_lastX = static_cast<float>(xpos);
        app->m_lastY = static_cast<float>(ypos);
        app->m_firstMouse = false;
    }
    
    float xoffset = static_cast<float>(xpos) - app->m_lastX;
    float yoffset = app->m_lastY - static_cast<float>(ypos); // Reversed since y-coordinates go from bottom to top
    
    app->m_lastX = static_cast<float>(xpos);
    app->m_lastY = static_cast<float>(ypos);
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        app->m_renderer->getCamera().processMouseMovement(xoffset, yoffset);
    }
}

void SandboxSimulation::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    
    if (!app->m_imguiManager->isMouseOverUI()) {
        app->m_renderer->getCamera().processMouseScroll(static_cast<float>(yoffset));
    }
}

void SandboxSimulation::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        // Trigger fission demonstration
        app->demonstrateFission();
    }
    
    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        // Trigger electron jump demonstration
        app->demonstrateElectronJump();
    }
}

// Main function
int main() {
    SandboxSimulation app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }
    
    app.run();
    
    return 0;
}