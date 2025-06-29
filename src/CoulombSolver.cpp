#include "CoulombSolver.h"

// Coulomb's constant (k_e) in N·m²/C²
const float COULOMB_CONSTANT = 8.9875e9f;

std::vector<glm::vec3> CoulombSolver::calculateForces(const std::vector<std::shared_ptr<Particle>>& particles) {
    std::vector<glm::vec3> forces(particles.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            // Calculate vector between particles
            glm::vec3 r_vec = particles[i]->getPosition() - particles[j]->getPosition();
            float distance = glm::length(r_vec);

            // Avoid division by zero if particles are at the same position
            if (distance < 1e-9f) { // Small epsilon to prevent singularity
                continue;
            }

            // Calculate force magnitude using Coulomb's Law: F = k * |q1 * q2| / r^2
            float forceMagnitude = COULOMB_CONSTANT * (particles[i]->getCharge() * particles[j]->getCharge()) / (distance * distance);

            // Calculate force direction (unit vector)
            glm::vec3 forceDirection = glm::normalize(r_vec);

            // Apply force to both particles
            // If charges are same sign, force is repulsive (away from each other)
            // If charges are opposite sign, force is attractive (towards each other)
            glm::vec3 force = forceMagnitude * forceDirection;

            forces[i] += force;
            forces[j] -= force; // Newton's third law
        }
    }
    return forces;
}


