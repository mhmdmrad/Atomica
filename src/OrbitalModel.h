#ifndef ORBITAL_MODEL_H
#define ORBITAL_MODEL_H

#include <memory>
#include "Atom.h"
#include "Particle.h"

/**
 * @brief Models discrete electron orbitals and transitions.
 * 
 * This class handles electron configurations, calculates energy levels,
 * and simulates electron jumps with associated photon energy changes.
 */
class OrbitalModel {
public:
    /**
     * @brief Constructs a new OrbitalModel object.
     */
    OrbitalModel() = default;

    /**
     * @brief Calculates the energy of an electron in a given orbital level for a hydrogen-like atom.
     * 
     * @param atomicNumber The atomic number (Z) of the atom.
     * @param orbitalLevel The principal quantum number (n) of the orbital.
     * @return The energy of the orbital in eV.
     */
    float calculateOrbitalEnergy(int atomicNumber, int orbitalLevel) const;

    /**
     * @brief Simulates an electron jump between two orbital levels.
     * 
     * @param electron A shared pointer to the electron undergoing the jump.
     * @param atom The atom to which the electron belongs.
     * @param newOrbitalLevel The target orbital level for the electron.
     * @return The energy difference (ΔE) of the photon emitted or absorbed in eV.
     *         Positive for absorption, negative for emission.
     */
    float simulateElectronJump(std::shared_ptr<Electron> electron, std::shared_ptr<Atom> atom, int newOrbitalLevel);

private:
    // Rydberg constant in eV
    static constexpr float RYDBERG_CONSTANT_EV = 13.605693f;
};

/// Convert photon energy ΔE (eV) to wavelength in nanometers:
static inline float energyToWavelengthNm(float deltaE_eV) {
    // E (eV) = 1240 nm·eV / λ (nm) → λ = 1240 / |ΔE|
    return deltaE_eV != 0.0f
      ? 1240.0f / std::abs(deltaE_eV)
      : INFINITY;
}

/// Classify the band (nm):
enum class Band { ULTRAVIOLET, VISIBLE, INFRARED };
static inline Band classifyBand(float wavelengthNm) {
    if (wavelengthNm < 380.0f)       return Band::ULTRAVIOLET;
    else if (wavelengthNm <= 750.0f) return Band::VISIBLE;
    else                               return Band::INFRARED;

#endif // ORBITAL_MODEL_H