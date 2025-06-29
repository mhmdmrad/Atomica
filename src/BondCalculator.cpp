#include "BondCalculator.h"
#include <iostream>

BondCalculator::BondCalculator() {
    // Initialize with example bond energies in eV
    m_bondEnergies[Bond::Type::SINGLE] = 4.5f;   // Example: C-C single bond ~348 kJ/mol = 3.6 eV
    m_bondEnergies[Bond::Type::DOUBLE] = 8.0f;   // Example: C=C double bond ~614 kJ/mol = 6.3 eV
    m_bondEnergies[Bond::Type::TRIPLE] = 10.0f;  // Example: C≡C triple bond ~839 kJ/mol = 8.7 eV
    m_bondEnergies[Bond::Type::IONIC] = 5.0f;    // Example: NaCl ionic bond
    m_bondEnergies[Bond::Type::METALLIC] = 2.0f; // Example: Metallic bond
    m_bondEnergies[Bond::Type::HYDROGEN] = 0.2f; // Example: Hydrogen bond
}

Bond::Type BondCalculator::determineBondType(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2) const {
    // This is a highly simplified model for demonstration purposes.
    // A real chemical bonding model would be far more complex, involving
    // electronegativity, orbital overlap, quantum mechanics, etc.

    // Example: If both are Hydrogen, assume a single bond
    if (atom1->getAtomicNumber() == 1 && atom2->getAtomicNumber() == 1) {
        return Bond::Type::SINGLE;
    }
    // Example: If one is Oxygen and other is Hydrogen, assume a single bond (like in H2O)
    else if ((atom1->getAtomicNumber() == 1 && atom2->getAtomicNumber() == 8) ||
             (atom1->getAtomicNumber() == 8 && atom2->getAtomicNumber() == 1)) {
        return Bond::Type::SINGLE;
    }
    // Example: If both are Oxygen, assume a double bond (like in O2)
    else if (atom1->getAtomicNumber() == 8 && atom2->getAtomicNumber() == 8) {
        return Bond::Type::DOUBLE;
    }
    // Example: If both are Nitrogen, assume a triple bond (like in N2)
    else if (atom1->getAtomicNumber() == 7 && atom2->getAtomicNumber() == 7) {
        return Bond::Type::TRIPLE;
    }
    // Default to single bond for any other combination for now
    return Bond::Type::SINGLE;
}

float BondCalculator::getBondEnergy(Bond::Type type) const {
    auto it = m_bondEnergies.find(type);
    if (it != m_bondEnergies.end()) {
        return it->second;
    }
    // Return a default or error value if bond type not found
    std::cerr << "Warning: Bond energy not found for type: " << static_cast<int>(type) << std::endl;
    return 0.0f; 
}

std::string BondCalculator::bondTypeToString(Bond::Type type) const {
    switch (type) {
        case Bond::Type::SINGLE: return "SINGLE";
        case Bond::Type::DOUBLE: return "DOUBLE";
        case Bond::Type::TRIPLE: return "TRIPLE";
        case Bond::Type::IONIC: return "IONIC";
        case Bond::Type::METALLIC: return "METALLIC";
        case Bond::Type::HYDROGEN: return "HYDROGEN";
        default: return "UNKNOWN";
    }
}


