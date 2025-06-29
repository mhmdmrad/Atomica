#include "Particle.h"

Particle::Particle(Type type, const glm::vec3& position, const glm::vec3& velocity, float mass, float charge)
    : m_type(type),
      m_position(position),
      m_velocity(velocity),
      m_mass(mass),
      m_charge(charge) {}

void Particle::update(const glm::vec3& force, float deltaTime) {
    // Basic Euler integration for now
    // F = ma => a = F/m
    glm::vec3 acceleration = force / m_mass;
    m_velocity += acceleration * deltaTime;
    m_position += m_velocity * deltaTime;
}

Nucleus::Nucleus(int atomicNumber, int massNumber, const glm::vec3& position, const glm::vec3& velocity, float mass, float charge)
    : Particle(Type::NUCLEUS, position, velocity, mass, charge),
      m_atomicNumber(atomicNumber),
      m_massNumber(massNumber) {}

Electron::Electron(const glm::vec3& position, const glm::vec3& velocity, int orbitalLevel)
    : Particle(Type::ELECTRON, position, velocity, 9.109e-31f, -1.602e-19f), // Mass and charge of electron
      m_orbitalLevel(orbitalLevel) {}
