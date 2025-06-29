#ifndef ATOM_H
#define ATOM_H

#include <vector>
#include <memory>
#include "Particle.h"

// Forward declarations
class Nucleus;
class Electron;

/**
 * @brief Represents an atom, composed of a nucleus and electrons.
 * 
 * This class manages the nucleus and electron particles that constitute an atom,
 * and provides methods for managing its state and electron configuration.
 */
class Atom {
public:
    /**
     * @brief Constructs a new Atom object.
     * 
     * @param atomicNumber The atomic number (number of protons) of the atom.
     * @param massNumber The mass number (protons + neutrons) of the atom.
     * @param position The initial position of the atom.
     */
    Atom(int atomicNumber, int massNumber, const glm::vec3& position);

    /**
     * @brief Destroys the Atom object.
     */
    ~Atom();

    /**
     * @brief Gets the atomic number of the atom.
     * 
     * @return The atomic number.
     */
    int getAtomicNumber() const { return m_atomicNumber; }

    /**
     * @brief Gets the mass number of the atom.
     * 
     * @return The mass number.
     */
    int getMassNumber() const { return m_massNumber; }

    /**
     * @brief Gets the nucleus of the atom.
     * 
     * @return A shared pointer to the Nucleus object.
     */
    std::shared_ptr<Nucleus> getNucleus() const { return m_nucleus; }

    /**
     * @brief Gets a constant reference to the list of electrons in the atom.
     * 
     * @return A constant reference to the vector of shared pointers to Electron objects.
     */
    const std::vector<std::shared_ptr<Electron>>& getElectrons() const { return m_electrons; }

    /**
     * @brief Gets the position of the atom (taken from its nucleus).
     * 
     * @return The atom's position.
     */
    const glm::vec3& getPosition() const;

    /**
     * @brief Sets the position of the atom (updates nucleus and electrons).
     * 
     * @param position The new position.
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Adds an electron to the atom.
     * 
     * @param electron A shared pointer to the electron to add.
     */
    void addElectron(std::shared_ptr<Electron> electron);

    /**
     * @brief Removes an electron from the atom.
     * 
     * @param electron A shared pointer to the electron to remove.
     * @return True if the electron was removed, false otherwise.
     */
    bool removeElectron(std::shared_ptr<Electron> electron);

private:
    int m_atomicNumber;
    int m_massNumber;
    std::shared_ptr<Nucleus> m_nucleus;
    std::vector<std::shared_ptr<Electron>> m_electrons;
};

#endif // ATOM_H


