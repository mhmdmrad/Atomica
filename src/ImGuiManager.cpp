#include "ImGuiManager.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>  // for glm::value_ptr if needed

ImGuiManager::ImGuiManager(GLFWwindow* window)
    : m_window(window) {}

ImGuiManager::~ImGuiManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool ImGuiManager::initialize() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // GLFW backend
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        std::cerr << "ImGui_ImplGlfw_InitForOpenGL failed\n";
        return false;
    }
    // OpenGL3 backend (requires a valid GL context and GLEW)
    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
        std::cerr << "ImGui_ImplOpenGL3_Init failed\n";
        return false;
    }
    std::cout << "ImGui initialized successfully\n";
    return true;
}

void ImGuiManager::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::render(PhysicsEngine& physicsEngine) {
    renderAtomPalette(physicsEngine);
    renderBondingControls(physicsEngine);
    renderNuclearControls(physicsEngine);
    renderOrbitalControls(physicsEngine);
    renderSimulationInfo(physicsEngine);
}

void ImGuiManager::endFrame() {
    ImGui::Render();

    // Disable depth-test so UI draws on top of 3D
    glDisable(GL_DEPTH_TEST);
    // Ensure alpha blending for UI
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Restore depth-test for subsequent 3D
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

bool ImGuiManager::isMouseOverUI() const {
    return ImGui::GetIO().WantCaptureMouse;
}

// — UI panels below —

void ImGuiManager::renderAtomPalette(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Atom Palette");
    ImGui::Text("Create Atoms");
    ImGui::Separator();

    const char* elements[] = {
        "Hydrogen","Helium","Lithium","Beryllium","Boron",
        "Carbon","Nitrogen","Oxygen","Fluorine","Neon"
    };
    static int elemIdx = 0;
    ImGui::Combo("Element", &elemIdx, elements, IM_ARRAYSIZE(elements));
    m_selectedAtomicNumber = elemIdx + 1;

    ImGui::InputInt("Mass Number", &m_selectedMassNumber);
    if (m_selectedMassNumber < m_selectedAtomicNumber)
        m_selectedMassNumber = m_selectedAtomicNumber;

    static float pos[3] = {0,0,0};
    ImGui::InputFloat3("Position", pos);

    if (ImGui::Button("Create Atom")) {
        auto atom = std::make_shared<Atom>(
            m_selectedAtomicNumber,
            m_selectedMassNumber,
            glm::vec3(pos[0],pos[1],pos[2])
        );
        physicsEngine.addAtom(atom);
        std::cout << "Created " << getElementName(m_selectedAtomicNumber) << "\n";
    }
    ImGui::End();
}

void ImGuiManager::renderBondingControls(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Bonding Controls");
    ImGui::Text("Bond Formation");
    ImGui::Separator();

    ImGui::Checkbox("Bonding Mode", &m_bondingMode);
    if (m_bondingMode) {
        ImGui::Text("Click on two atoms to create a bond");
        if (ImGui::Button("Clear Selection")) {
            m_selectedAtom1.reset();
            m_selectedAtom2.reset();
        }
        // fallback: bond first two atoms
        const auto& atoms = physicsEngine.getAtoms();
        if (atoms.size()>=2 && ImGui::Button("Bond First Two")) {
            auto mol = std::make_shared<Molecule>();
            mol->addAtom(atoms[0]);
            mol->addAtom(atoms[1]);
            BondCalculator bc;
            auto type  = bc.determineBondType(atoms[0], atoms[1]);
            auto energy= bc.getBondEnergy(type);
            mol->addBond(std::make_shared<Bond>(atoms[0],atoms[1],type,energy));
            physicsEngine.addMolecule(mol);
            std::cout<<"Bonded: "<<energy<<" eV\n";
        }
    }
    ImGui::End();
}

void ImGuiManager::renderNuclearControls(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Nuclear Controls");
    ImGui::Text("Nuclear Reactions");
    ImGui::Separator();

    ImGui::Checkbox("Fission Mode", &m_fissionMode);
    ImGui::Checkbox("Fusion Mode", &m_fusionMode);

    if (m_fissionMode) {
        const auto& atoms = physicsEngine.getAtoms();
        for (auto& atom: atoms) {
            if (atom->getAtomicNumber()>=90 && ImGui::Button("Trigger Fission")) {
                NuclearReactor nr;
                float e = nr.simulateFission(atom->getNucleus());
                std::cout<<"Fission: "<<e<<" eV\n";
                break;
            }
        }
    }
    if (m_fusionMode) {
        const auto& atoms = physicsEngine.getAtoms();
        if (atoms.size()>=2 && ImGui::Button("Trigger Fusion")) {
            NuclearReactor nr;
            float e = nr.simulateFusion(
                atoms[0]->getNucleus(),
                atoms[1]->getNucleus()
            );
            std::cout<<"Fusion: "<<e<<" eV\n";
        }
    }
    ImGui::End();
}

void ImGuiManager::renderOrbitalControls(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Orbital Controls");
    ImGui::Text("Electron Transitions");
    ImGui::Separator();

    ImGui::InputInt("Current Orbital",&m_selectedElectronOrbital);
    ImGui::InputInt("Target Orbital",&m_targetOrbital);
    if (m_targetOrbital<1) m_targetOrbital=1;

    const auto& atoms = physicsEngine.getAtoms();
    if (!atoms.empty() && ImGui::Button("Trigger Electron Jump")) {
        auto& elecs = atoms[0]->getElectrons();
        if (!elecs.empty()) {
            OrbitalModel om;
            float dE = om.simulateElectronJump(
                elecs[0], atoms[0], m_targetOrbital
            );
            std::cout<<"ΔE: "<<dE<<" eV\n";
        }
    }
    ImGui::End();
}

void ImGuiManager::renderSimulationInfo(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Simulation Info");
    auto& A = physicsEngine.getAtoms();
    auto& M = physicsEngine.getMolecules();
    ImGui::Text("Atoms: %zu", A.size());
    ImGui::Text("Molecules: %zu", M.size());
    ImGui::Separator();
    ImGui::Text("Use mouse & scroll to navigate");
    ImGui::End();
}

std::string ImGuiManager::getElementName(int atomicNumber) const {
    static const char* names[] = {
        "", "Hydrogen","Helium","Lithium","Beryllium","Boron",
        "Carbon","Nitrogen","Oxygen","Fluorine","Neon"
    };
    if (atomicNumber>=1 && atomicNumber<=10)
        return names[atomicNumber];
    return "Unknown";
}