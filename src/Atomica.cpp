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

class SandboxSimulation {
public:
    SandboxSimulation();
    ~SandboxSimulation();
    bool initialize();
    void run();

private:
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<ImGuiManager> m_imguiManager;
    std::unique_ptr<PhysicsEngine> m_physicsEngine;

    bool m_running = false;
    int m_windowWidth = 1200;
    int m_windowHeight = 800;

    bool m_firstMouse = true;
    float m_lastX = 0.0f, m_lastY = 0.0f;

    bool initializeWindow();
    bool initializeOpenGL();
    void setupScene();
    void demonstrateH2OMolecule();
    void demonstrateFission();
    void demonstrateElectronJump();
    void update(float deltaTime);
    void render(float deltaTime);
    void handleInput();
    void cleanup();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

SandboxSimulation::SandboxSimulation() {}

SandboxSimulation::~SandboxSimulation() {
    cleanup();
}

bool SandboxSimulation::initialize() {
    Logger::getInstance().setLogLevel(Logger::Level::INFO);
    Logger::getInstance().setLogFile("simulation.log");

    if (!initializeWindow()) return false;
    if (!initializeOpenGL()) return false;

    m_renderer = std::make_unique<Renderer>(m_window);
    if (!m_renderer->initialize()) return false;

    m_imguiManager = std::make_unique<ImGuiManager>(m_window);
    if (!m_imguiManager->initialize()) return false;

    m_physicsEngine = std::make_unique<PhysicsEngine>();

    setupScene();

    // make sure camera is a good distance
    m_renderer->getCamera().setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    m_running = true;

    return true;
}

void SandboxSimulation::run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (m_running && !glfwWindowShouldClose(m_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        handleInput();
        update(deltaTime);
        render(deltaTime);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

bool SandboxSimulation::initializeWindow() {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Sandbox Simulation", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return true;
}

bool SandboxSimulation::initializeOpenGL() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    return err == GLEW_OK;
}

void SandboxSimulation::setupScene() {
    demonstrateH2OMolecule();

    auto carbon = std::make_shared<Atom>(6, 12, glm::vec3(3.0f, 0.0f, 0.0f));
    auto nitrogen = std::make_shared<Atom>(7, 14, glm::vec3(-3.0f, 0.0f, 0.0f));
    m_physicsEngine->addAtom(carbon);
    m_physicsEngine->addAtom(nitrogen);
}

void SandboxSimulation::demonstrateH2OMolecule() {
    auto oxygen = std::make_shared<Atom>(8, 16, glm::vec3(0.0f, 0.0f, 0.0f));
    auto hydrogen1 = std::make_shared<Atom>(1, 1, glm::vec3(1.0f, 0.5f, 0.0f));
    auto hydrogen2 = std::make_shared<Atom>(1, 1, glm::vec3(-1.0f, 0.5f, 0.0f));

    auto h2o = std::make_shared<Molecule>();
    h2o->addAtom(oxygen);
    h2o->addAtom(hydrogen1);
    h2o->addAtom(hydrogen2);

    BondCalculator bondCalc;
    auto bond1 = std::make_shared<Bond>(oxygen, hydrogen1, bondCalc.determineBondType(oxygen, hydrogen1),
                                        bondCalc.getBondEnergy(bondCalc.determineBondType(oxygen, hydrogen1)));
    auto bond2 = std::make_shared<Bond>(oxygen, hydrogen2, bondCalc.determineBondType(oxygen, hydrogen2),
                                        bondCalc.getBondEnergy(bondCalc.determineBondType(oxygen, hydrogen2)));
    h2o->addBond(bond1);
    h2o->addBond(bond2);

    m_physicsEngine->addMolecule(h2o);

    m_renderer->addEnergyLabel(glm::vec3(0.5f, 0.25f, 0.0f), bond1->getEnergy(), 5.0f);
    m_renderer->addEnergyLabel(glm::vec3(-0.5f, 0.25f, 0.0f), bond2->getEnergy(), 5.0f);
}

void SandboxSimulation::update(float deltaTime) {
    m_physicsEngine->update(deltaTime);
}

void SandboxSimulation::render(float deltaTime) {
    m_imguiManager->newFrame();

    m_renderer->render(
      m_physicsEngine->getAtoms(),
      m_physicsEngine->getMolecules(),
      deltaTime
    );

    m_imguiManager->render(*m_physicsEngine);

    m_imguiManager->endFrame();
}

void SandboxSimulation::handleInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_running = false;
}

void SandboxSimulation::cleanup() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void SandboxSimulation::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    app->m_windowWidth = width;
    app->m_windowHeight = height;
    app->m_renderer->onWindowResize(width, height);
}

void SandboxSimulation::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* app = static_cast<SandboxSimulation*>(glfwGetWindowUserPointer(window));
    if (app->m_firstMouse) {
        app->m_lastX = static_cast<float>(xpos);
        app->m_lastY = static_cast<float>(ypos);
        app->m_firstMouse = false;
    }
}

void SandboxSimulation::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}

void SandboxSimulation::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

int main() {
    SandboxSimulation app;
    if (!app.initialize()) return -1;
    app.run();
    return 0;
}