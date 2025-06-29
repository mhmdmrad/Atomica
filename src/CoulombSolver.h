#ifndef COULOMB_SOLVER_H
#define COULOMB_SOLVER_H

#include <vector>
#include <memory>
#include "Particle.h"

/**
 * @brief Solves Coulombic forces between charged particles.
 * 
 * This class provides methods to calculate the electrostatic forces
 * between a collection of Particle objects.
 */
class CoulombSolver {
public:
    /**
     * @brief Constructs a new CoulombSolver object.
     */
    CoulombSolver() = default;

    /**
     * @brief Calculates the total electrostatic force on each particle.
     * 
     * @param particles A vector of shared pointers to Particle objects.
     * @return A vector of glm::vec3, where each element is the total force
     *         on the corresponding particle in the input vector.
     */
    std::vector<glm::vec3> calculateForces(const std::vector<std::shared_ptr<Particle>>& particles);

private:
    // TODO: Implement Barnes-Hut for optimization if needed.
};

#endif // COULOMB_SOLVER_H


