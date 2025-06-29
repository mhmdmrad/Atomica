#include "OrbitalModel.h"
#include <iostream>
#include <cmath>

float OrbitalModel::calculateOrbitalEnergy(int atomicNumber, int orbitalLevel) const {
    if (orbitalLevel <= 0) {
        std::cerr << "Error: Orbital level must be a positive integer." << std::endl;
        return 0.0f;
    }
    // For hydrogen-like atoms: E = -Rydberg * (Z^2 / n^2)
    return -RYDBERG_CONSTANT_EV * (static_cast<float>(atomicNumber * atomicNumber) / (orbitalLevel * orbitalLevel));
}

float OrbitalModel::simulateElectronJump(std::shared_ptr<Electron> electron, std::shared_ptr<Atom> atom, int newOrbitalLevel) {
    if (!electron || !atom) {
        std::cerr << "Error: Invalid electron or atom provided for electron jump simulation." << std::endl;
        return 0.0f;
    }

    int currentOrbitalLevel = electron->getOrbitalLevel();
    if (newOrbitalLevel <= 0) {
        std::cerr << "Error: New orbital level must be a positive integer." << std::endl;
        return 0.0f;
    }

    // Calculate initial and final energies
    float initialEnergy = calculateOrbitalEnergy(atom->getAtomicNumber(), currentOrbitalLevel);
    float finalEnergy = calculateOrbitalEnergy(atom->getAtomicNumber(), newOrbitalLevel);

    // Calculate energy difference (ΔE)
    // If finalEnergy > initialEnergy, energy is absorbed (positive ΔE)
    // If finalEnergy < initialEnergy, energy is emitted (negative ΔE)
    float deltaE = finalEnergy - initialEnergy;

    // Update the electron's orbital level
    electron->setOrbitalLevel(newOrbitalLevel);

    std::cout << "Electron jumped from orbital " << currentOrbitalLevel << " to " << newOrbitalLevel
              << " in atom with Z=" << atom->getAtomicNumber() << ". ΔE = " << deltaE << " eV" << std::endl;

    return deltaE;
}


