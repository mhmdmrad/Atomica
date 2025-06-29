#ifndef MOLECULE_H
#define MOLECULE_H

#include <vector>
#include <memory>
#include "Atom.h"
#include "Bond.h"

/**
 * @brief Represents a molecule composed of atoms and bonds.
 * 
 * This class manages a collection of Atom and Bond objects, providing
 * functionalities to add/remove them and potentially calculate molecular properties.
 */
class Molecule {
public:
    /**
     * @brief Constructs a new Molecule object.
     */
    Molecule() = default;

    /**
     * @brief Adds an atom to the molecule.
     * 
     * @param atom A shared pointer to the atom to add.
     */
    void addAtom(std::shared_ptr<Atom> atom);

    /**
     * @brief Adds a bond to the molecule.
     * 
     * @param bond A shared pointer to the bond to add.
     */
    void addBond(std::shared_ptr<Bond> bond);

    /**
     * @brief Gets a constant reference to the list of atoms in the molecule.
     * 
     * @return A constant reference to the vector of shared pointers to atoms.
     */
    const std::vector<std::shared_ptr<Atom>>& getAtoms() const { return m_atoms; }

    /**
     * @brief Gets a constant reference to the list of bonds in the molecule.
     * 
     * @return A constant reference to the vector of shared pointers to bonds.
     */
    const std::vector<std::shared_ptr<Bond>>& getBonds() const { return m_bonds; }

private:
    std::vector<std::shared_ptr<Atom>> m_atoms;
    std::vector<std::shared_ptr<Bond>> m_bonds;
};

#endif // MOLECULE_H


