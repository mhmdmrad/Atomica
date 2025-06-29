#include "ImGuiManager.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <iostream>

ImGuiManager::ImGuiManager(GLFWwindow* window)
    : m_window(window),
      m_selectedAtomicNumber(1),
      m_selectedMassNumber(1),
      m_bondingMode(false),
      m_fissionMode(false),
      m_fusionMode(false),
      m_selectedElectronOrbital(1),
      m_targetOrbital(2) {}

ImGuiManager::~ImGuiManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool ImGuiManager::initialize() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        return false;
    }

    std::cout << "ImGui initialized successfully" << std::endl;
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
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiManager::isMouseOverUI() const {
    return ImGui::GetIO().WantCaptureMouse;
}

void ImGuiManager::renderAtomPalette(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Atom Palette");
    
    ImGui::Text("Create Atoms");
    ImGui::Separator();
    
    // Element selection
    const char* elements[] = {"Hydrogen", "Helium", "Lithium", "Beryllium", "Boron", "Carbon", "Nitrogen", "Oxygen"};
    static int selectedElement = 0;
    ImGui::Combo("Element", &selectedElement, elements, IM_ARRAYSIZE(elements));
    m_selectedAtomicNumber = selectedElement + 1;
    
    // Mass number input
    ImGui::InputInt("Mass Number", &m_selectedMassNumber);
    if (m_selectedMassNumber < m_selectedAtomicNumber) {
        m_selectedMassNumber = m_selectedAtomicNumber;
    }
    
    // Position inputs
    static float position[3] = {0.0f, 0.0f, 0.0f};
    ImGui::InputFloat3("Position", position);
    
    if (ImGui::Button("Create Atom")) {
        auto atom = std::make_shared<Atom>(m_selectedAtomicNumber, m_selectedMassNumber, 
                                          glm::vec3(position[0], position[1], position[2]));
        physicsEngine.addAtom(atom);
        std::cout << "Created " << getElementName(m_selectedAtomicNumber) << " atom" << std::endl;
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
        
        // TODO: Implement atom selection via mouse clicks
        // For now, we'll create bonds between first two atoms if they exist
        const auto& atoms = physicsEngine.getAtoms();
        if (atoms.size() >= 2 && ImGui::Button("Bond First Two Atoms")) {
            // Create a simple molecule with the first two atoms
            auto molecule = std::make_shared<Molecule>();
            molecule->addAtom(atoms[0]);
            molecule->addAtom(atoms[1]);
            
            // Determine bond type and energy
            BondCalculator bondCalc;
            Bond::Type bondType = bondCalc.determineBondType(atoms[0], atoms[1]);
            float bondEnergy = bondCalc.getBondEnergy(bondType);
            
            auto bond = std::make_shared<Bond>(atoms[0], atoms[1], bondType, bondEnergy);
            molecule->addBond(bond);
            
            physicsEngine.addMolecule(molecule);
            std::cout << "Created bond between atoms with energy: " << bondEnergy << " eV" << std::endl;
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
        ImGui::Text("Click on a heavy nucleus to trigger fission");
        
        // Simulate fission on first heavy atom (if exists)
        const auto& atoms = physicsEngine.getAtoms();
        for (const auto& atom : atoms) {
            if (atom->getAtomicNumber() >= 90) { // Heavy elements
                if (ImGui::Button("Trigger Fission")) {
                    NuclearReactor reactor;
                    float energy = reactor.simulateFission(atom->getNucleus());
                    std::cout << "Fission energy released: " << energy << " eV" << std::endl;
                    // TODO: Add energy label to renderer
                }
                break;
            }
        }
    }
    
    if (m_fusionMode) {
        ImGui::Text("Select two light nuclei for fusion");
        
        // Simulate fusion between first two light atoms (if exist)
        const auto& atoms = physicsEngine.getAtoms();
        if (atoms.size() >= 2) {
            bool foundLightNuclei = false;
            for (size_t i = 0; i < atoms.size() - 1; ++i) {
                if (atoms[i]->getAtomicNumber() <= 2 && atoms[i+1]->getAtomicNumber() <= 2) {
                    if (ImGui::Button("Trigger Fusion")) {
                        NuclearReactor reactor;
                        float energy = reactor.simulateFusion(atoms[i]->getNucleus(), atoms[i+1]->getNucleus());
                        std::cout << "Fusion energy released: " << energy << " eV" << std::endl;
                        // TODO: Add energy label to renderer
                    }
                    foundLightNuclei = true;
                    break;
                }
            }
            if (!foundLightNuclei) {
                ImGui::Text("No suitable light nuclei found");
            }
        }
    }
    
    ImGui::End();
}

void ImGuiManager::renderOrbitalControls(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Orbital Controls");
    
    ImGui::Text("Electron Transitions");
    ImGui::Separator();
    
    ImGui::InputInt("Current Orbital", &m_selectedElectronOrbital);
    ImGui::InputInt("Target Orbital", &m_targetOrbital);
    
    if (m_selectedElectronOrbital < 1) m_selectedElectronOrbital = 1;
    if (m_targetOrbital < 1) m_targetOrbital = 1;
    
    const auto& atoms = physicsEngine.getAtoms();
    if (!atoms.empty()) {
        if (ImGui::Button("Trigger Electron Jump")) {
            auto& electrons = atoms[0]->getElectrons();
            if (!electrons.empty()) {
                OrbitalModel orbitalModel;
                float deltaE = orbitalModel.simulateElectronJump(electrons[0], atoms[0], m_targetOrbital);
                std::cout << "Electron transition ΔE: " << deltaE << " eV" << std::endl;
                // TODO: Add energy label to renderer
            }
        }
    } else {
        ImGui::Text("No atoms available for electron transitions");
    }
    
    ImGui::End();
}

void ImGuiManager::renderSimulationInfo(PhysicsEngine& physicsEngine) {
    ImGui::Begin("Simulation Info");
    
    const auto& atoms = physicsEngine.getAtoms();
    const auto& molecules = physicsEngine.getMolecules();
    
    ImGui::Text("Atoms: %zu", atoms.size());
    ImGui::Text("Molecules: %zu", molecules.size());
    
    ImGui::Separator();
    ImGui::Text("Controls:");
    ImGui::Text("Mouse: Rotate camera");
    ImGui::Text("Scroll: Zoom in/out");
    
    ImGui::End();
}

std::string ImGuiManager::getElementName(int atomicNumber) const {
    const char* names[] = {
        "", "Hydrogen", "Helium", "Lithium", "Beryllium", "Boron", 
        "Carbon", "Nitrogen", "Oxygen", "Fluorine", "Neon"
    };
    
    if (atomicNumber >= 1 && atomicNumber <= 10) {
        return names[atomicNumber];
    }
    return "Unknown";
}

