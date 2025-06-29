#ifndef NUCLEAR_REACTOR_H
#define NUCLEAR_REACTOR_H

#include <memory>
#include <vector>
#include "Particle.h"
#include "Atom.h"

/**
 * @brief Simulates nuclear fission and fusion events.
 * 
 * This class contains kernels for triggering and calculating the outcomes
 * of nuclear reactions, including energy release and byproduct generation.
 */
class NuclearReactor {
public:
    /**
     * @brief Constructs a new NuclearReactor object.
     */
    NuclearReactor() = default;

    /**
     * @brief Simulates a nuclear fission event.
     * 
     * @param nucleus The nucleus undergoing fission.
     * @return The energy released (ΔE) in eV.
     */
    float simulateFission(std::shared_ptr<Nucleus> nucleus);

    /**
     * @brief Simulates a nuclear fusion event.
     * 
     * @param nucleus1 The first nucleus involved in fusion.
     * @param nucleus2 The second nucleus involved in fusion.
     * @return The energy released (ΔE) in eV.
     */
    float simulateFusion(std::shared_ptr<Nucleus> nucleus1, std::shared_ptr<Nucleus> nucleus2);

private:
    // Helper function to calculate binding energy per nucleon (simplified)
    float calculateBindingEnergyPerNucleon(int atomicNumber, int massNumber) const;
};

#endif // NUCLEAR_REACTOR_H


