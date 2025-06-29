#include "NuclearReactor.h"
#include <iostream>
#include <cmath>

// Constants
const float AMU_TO_KG = 1.660539e-27f; // Atomic mass unit to kilograms
const float C_SQUARED = 8.98755179e16f; // Speed of light squared (c^2) in m^2/s^2
const float J_TO_EV = 6.242e18f; // Joules to electronVolts

// Simplified binding energy per nucleon curve approximation
// This is a very rough approximation for demonstration and will not be perfectly accurate.
// Real binding energy curve is more complex.
float NuclearReactor::calculateBindingEnergyPerNucleon(int atomicNumber, int massNumber) const {
    if (massNumber == 0) return 0.0f;

    // A very simplified approximation of the binding energy per nucleon curve
    // Peaks around A=56 (Iron)
    float A = static_cast<float>(massNumber);
    float Z = static_cast<float>(atomicNumber);

    // Liquid Drop Model inspired terms (highly simplified coefficients)
    float volumeTerm = 15.7f * A; // MeV
    float surfaceTerm = 17.8f * std::pow(A, 2.0f/3.0f); // MeV
    float coulombTerm = 0.71f * Z * (Z - 1) / std::pow(A, 1.0f/3.0f); // MeV
    float asymmetryTerm = 23.7f * std::pow((A - 2*Z), 2.0f) / A; // MeV

    // Pairing term (simplified, just a general value for even-even, odd-odd, even-odd)
    float pairingTerm = 0.0f;
    if (massNumber % 2 == 0 && atomicNumber % 2 == 0) { // Even-Even
        pairingTerm = 11.2f / std::sqrt(A); // MeV
    } else if (massNumber % 2 != 0 && atomicNumber % 2 != 0) { // Odd-Odd
        pairingTerm = -11.2f / std::sqrt(A); // MeV
    } // Even-Odd or Odd-Even is 0

    float bindingEnergyMeV = volumeTerm - surfaceTerm - coulombTerm - asymmetryTerm + pairingTerm;

    if (bindingEnergyMeV < 0) bindingEnergyMeV = 0; // Binding energy cannot be negative

    return (bindingEnergyMeV / A) * 1e6f; // Convert MeV/nucleon to eV/nucleon
}

float NuclearReactor::simulateFission(std::shared_ptr<Nucleus> nucleus) {
    // Simplified fission of a heavy nucleus (e.g., Uranium-235)
    // U-235 (92 protons, 143 neutrons) -> Ba-141 (56 protons, 85 neutrons) + Kr-92 (36 protons, 56 neutrons) + 3 neutrons

    if (nucleus->getAtomicNumber() != 92 || nucleus->getMassNumber() != 235) {
        std::cerr << "Fission currently only supported for U-235 (simplified model)." << std::endl;
        return 0.0f;
    }

    // Masses in AMU (approximate values)
    float mass_U235 = 235.0439299f; // U-235
    float mass_Ba141 = 140.914411f; // Ba-141
    float mass_Kr92 = 91.926156f;   // Kr-92
    float mass_neutron = 1.008665f; // Neutron

    float initialMass = mass_U235;
    float finalMass = mass_Ba141 + mass_Kr92 + (3 * mass_neutron);

    float massDefectAMU = initialMass - finalMass;

    if (massDefectAMU <= 0) {
        std::cerr << "Warning: Fission resulted in non-positive mass defect. No energy released." << std::endl;
        return 0.0f;
    }

    // E = mc^2
    float energyJoules = massDefectAMU * AMU_TO_KG * C_SQUARED;
    float energyEV = energyJoules * J_TO_EV;

    std::cout << "Fission of U-235: Mass Defect = " << massDefectAMU << " AMU, Energy Released = " << energyEV << " eV" << std::endl;

    // In a real simulation, you would also create the new nuclei and neutrons.
    // For this simplified model, we just return the energy.
    return energyEV;
}

float NuclearReactor::simulateFusion(std::shared_ptr<Nucleus> nucleus1, std::shared_ptr<Nucleus> nucleus2) {
    // Simplified fusion of Deuterium and Tritium
    // D (1 proton, 1 neutron) + T (1 proton, 2 neutrons) -> He-4 (2 protons, 2 neutrons) + 1 neutron

    bool isDeuterium = (nucleus1->getAtomicNumber() == 1 && nucleus1->getMassNumber() == 2) ||
                       (nucleus2->getAtomicNumber() == 1 && nucleus2->getMassNumber() == 2);
    bool isTritium = (nucleus1->getAtomicNumber() == 1 && nucleus1->getMassNumber() == 3) ||
                     (nucleus2->getAtomicNumber() == 1 && nucleus2->getMassNumber() == 3);

    if (!(isDeuterium && isTritium)) {
        std::cerr << "Fusion currently only supported for Deuterium-Tritium (simplified model)." << std::endl;
        return 0.0f;
    }

    // Masses in AMU (approximate values)
    float mass_D = 2.01410178f; // Deuterium
    float mass_T = 3.01604927f; // Tritium
    float mass_He4 = 4.00260325f; // Helium-4
    float mass_neutron = 1.008665f; // Neutron

    float initialMass = mass_D + mass_T;
    float finalMass = mass_He4 + mass_neutron;

    float massDefectAMU = initialMass - finalMass;

    if (massDefectAMU <= 0) {
        std::cerr << "Warning: Fusion resulted in non-positive mass defect. No energy released." << std::endl;
        return 0.0f;
    }

    // E = mc^2
    float energyJoules = massDefectAMU * AMU_TO_KG * C_SQUARED;
    float energyEV = energyJoules * J_TO_EV;

    std::cout << "Fusion of D-T: Mass Defect = " << massDefectAMU << " AMU, Energy Released = " << energyEV << " eV" << std::endl;

    // In a real simulation, you would also create the new nuclei and neutrons.
    // For this simplified model, we just return the energy.
    return energyEV;
}


