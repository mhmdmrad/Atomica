#pragma once

#include <glm/glm.hpp>

/**
 * @brief Represents a generic particle in the simulation.
 */
class Particle {
public:
    enum class Type {
        PROTON,
        NEUTRON,
        ELECTRON,
        NUCLEUS
    };

    Particle(Type type, const glm::vec3& position, const glm::vec3& velocity, float mass, float charge);
    virtual ~Particle() = default;

    Type getType() const { return m_type; }
    const glm::vec3& getPosition() const { return m_position; }
    void setPosition(const glm::vec3& position) { m_position = position; }

    const glm::vec3& getVelocity() const { return m_velocity; }
    void setVelocity(const glm::vec3& velocity) { m_velocity = velocity; }

    float getMass() const { return m_mass; }
    float getCharge() const { return m_charge; }

    void update(const glm::vec3& force, float deltaTime);

protected:
    Type m_type;
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_mass;
    float m_charge;
};

/**
 * @brief Represents an atomic nucleus.
 */
class Nucleus : public Particle {
public:
    Nucleus(int atomicNumber, int massNumber, const glm::vec3& position, const glm::vec3& velocity, float mass, float charge);
    int getAtomicNumber() const { return m_atomicNumber; }
    int getMassNumber() const { return m_massNumber; }

private:
    int m_atomicNumber;
    int m_massNumber;
};

/**
 * @brief Represents an electron.
 */
class Electron : public Particle {
public:
    Electron(const glm::vec3& position, const glm::vec3& velocity, int orbitalLevel);
    int getOrbitalLevel() const { return m_orbitalLevel; }
    void setOrbitalLevel(int orbitalLevel) { m_orbitalLevel = orbitalLevel; }

private:
    int m_orbitalLevel;
};
