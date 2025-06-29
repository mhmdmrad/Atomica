#ifndef BOND_CALCULATOR_H
#define BOND_CALCULATOR_H

#include <memory>
#include <string>
#include <unordered_map>
#include "Bond.h"
#include "Atom.h"

/**
 * @brief Calculates and manages bond energies.
 * 
 * This class provides methods to determine bond types and their associated
 * energies, potentially using tabulated data.
 */
class BondCalculator {
public:
    /**
     * @brief Constructs a new BondCalculator object.
     */
    BondCalculator();

    /**
     * @brief Determines the bond type between two atoms.
     * 
     * @param atom1 A shared pointer to the first atom.
     * @param atom2 A shared pointer to the second atom.
     * @return The determined bond type.
     */
    Bond::Type determineBondType(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2) const;

    /**
     * @brief Gets the energy for a given bond type.
     * 
     * @param type The type of the bond.
     * @return The energy of the bond in eV.
     */
    float getBondEnergy(Bond::Type type) const;

private:
    // Tabulated bond energies (example values, will need realistic data)
    std::unordered_map<Bond::Type, float> m_bondEnergies;

    // Helper to convert bond type enum to string for map key (if needed for more complex mapping)
    std::string bondTypeToString(Bond::Type type) const;
};

#endif // BOND_CALCULATOR_H


