#include "PhysicsEngine.h"
#include <iostream>

PhysicsEngine::PhysicsEngine() {
    // Constructor, sub-modules are default constructed
}

void PhysicsEngine::addAtom(std::shared_ptr<Atom> atom) {
    m_atoms.push_back(atom);
}

void PhysicsEngine::addMolecule(std::shared_ptr<Molecule> molecule) {
    m_molecules.push_back(molecule);
    // Add all atoms from the molecule to the engine's atom list
    for (const auto& atom : molecule->getAtoms()) {
        addAtom(atom);
    }
}

void PhysicsEngine::update(float deltaTime) {
    // 1. Gather all particles for Coulomb force calculation
    std::vector<std::shared_ptr<Particle>> allParticles;
    for (const auto& atom : m_atoms) {
        allParticles.push_back(atom->getNucleus());
        for (const auto& electron : atom->getElectrons()) {
            allParticles.push_back(electron);
        }
    }

    // 2. Calculate Coulomb forces
    std::vector<glm::vec3> forces = m_coulombSolver.calculateForces(allParticles);

    // 3. Update particle positions and velocities
    for (size_t i = 0; i < allParticles.size(); ++i) {
        allParticles[i]->update(forces[i], deltaTime);
    }

    // 4. (TODO) Update bond energies (e.g., if atoms move too far apart, bond breaks)
    // This would involve iterating through m_bonds in m_molecules and checking distances.
    // For now, bond energies are static once set.

    // 5. (TODO) Check for nuclear events (fission/fusion)
    // This would involve checking proximity of nuclei and energy thresholds.
    // For now, these are triggered explicitly in main.cpp for demonstration.

    // 6. (TODO) Check for electron jumps
    // This would involve checking for energy input/output or explicit triggers.
    // For now, these are triggered explicitly in main.cpp for demonstration.
}


