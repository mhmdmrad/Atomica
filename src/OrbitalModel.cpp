#include "OrbitalModel.h"
#include <iostream>
#include <cmath>
#include <limits>

static float energyToWavelengthNm(float deltaE_eV) {
    if (deltaE_eV == 0.0f) return std::numeric_limits<float>::infinity();
    return 1240.0f / std::abs(deltaE_eV);
}

static OrbitalModel::Band classifyBand(float wavelengthNm) {
    if (wavelengthNm < 380.0f)       return OrbitalModel::Band::ULTRAVIOLET;
    else if (wavelengthNm <= 750.0f) return OrbitalModel::Band::VISIBLE;
    else                              return OrbitalModel::Band::INFRARED;
}

float OrbitalModel::calculateOrbitalEnergy(int atomicNumber,
                                           int orbitalLevel) const {
    if (orbitalLevel <= 0) {
        std::cerr << "Error: Orbital level must be a positive integer.\n";
        return 0.0f;
    }
    // E = -Rydberg * (Z² / n²)
    return -RYDBERG_CONSTANT_EV
           * (float(atomicNumber*atomicNumber)
              / float(orbitalLevel*orbitalLevel));
}

float OrbitalModel::simulateElectronJump(
    std::shared_ptr<Electron> electron,
    std::shared_ptr<Atom> atom,
    int newOrbitalLevel)
{
    if (!electron || !atom) {
        std::cerr << "Error: Invalid electron or atom.\n";
        return 0.0f;
    }

    int oldLevel = electron->getOrbitalLevel();
    if (newOrbitalLevel <= 0) {
        std::cerr << "Error: New orbital level must be > 0.\n";
        return 0.0f;
    }

    float E_i = calculateOrbitalEnergy(
                    atom->getAtomicNumber(), oldLevel);
    float E_f = calculateOrbitalEnergy(
                    atom->getAtomicNumber(), newOrbitalLevel);
    float deltaE = E_f - E_i;

    // update the electron
    electron->setOrbitalLevel(newOrbitalLevel);

    // compute wavelength & band
    float λ    = energyToWavelengthNm(deltaE);
    Band  band = classifyBand(λ);

    // log it
    std::cout
      << "Electron: n=" << oldLevel
      << " → " << newOrbitalLevel
      << " (Z=" << atom->getAtomicNumber() << "); "
      << "ΔE=" << deltaE << " eV, "
      << "λ="
      << (std::isfinite(λ)
            ? std::to_string(λ)+" nm"
            : std::string("∞"))
      << " (" 
      << (band==Band::VISIBLE ? "VISIBLE"
           : band==Band::ULTRAVIOLET ? "UV" 
                                      : "IR")
      << ")\n";

    return deltaE;
}