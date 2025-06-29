#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <vector>
#include <memory>
#include "Particle.h"
#include "Atom.h"
#include "Molecule.h"
#include "Bond.h"
#include "CoulombSolver.h"
#include "BondCalculator.h"
#include "NuclearReactor.h"
#include "OrbitalModel.h"

/**
 * @brief The main physics engine for the simulation.
 * 
 * This class orchestrates all physical interactions, including electromagnetic
 * forces, bond energy calculations, nuclear events, and electron transitions.
 */
class PhysicsEngine {
public:
    /**
     * @brief Constructs a new PhysicsEngine object.
     */
    PhysicsEngine();

    /**
     * @brief Adds an atom to the physics engine for simulation.
     * 
     * @param atom A shared pointer to the atom to add.
     */
    void addAtom(std::shared_ptr<Atom> atom);

    /**
     * @brief Adds a molecule to the physics engine for simulation.
     * 
     * @param molecule A shared pointer to the molecule to add.
     */
    void addMolecule(std::shared_ptr<Molecule> molecule);

    /**
     * @brief Updates the state of all simulated entities for a given time step.
     * 
     * This method will trigger force calculations, bond energy updates,
     * nuclear event checks, and electron transitions.
     * 
     * @param deltaTime The time step for the simulation update.
     */
    void update(float deltaTime);

    /**
     * @brief Gets a constant reference to the list of atoms managed by the engine.
     * 
     * @return A constant reference to the vector of shared pointers to atoms.
     */
    const std::vector<std::shared_ptr<Atom>>& getAtoms() const { return m_atoms; }

    /**
     * @brief Gets a constant reference to the list of molecules managed by the engine.
     * 
     * @return A constant reference to the vector of shared pointers to molecules.
     */
    const std::vector<std::shared_ptr<Molecule>>& getMolecules() const { return m_molecules; }

private:
    std::vector<std::shared_ptr<Atom>> m_atoms;
    std::vector<std::shared_ptr<Molecule>> m_molecules;

    // Physics sub-modules
    CoulombSolver m_coulombSolver;
    BondCalculator m_bondCalculator;
    NuclearReactor m_nuclearReactor;
    OrbitalModel m_orbitalModel;
};

#endif // PHYSICS_ENGINE_H


