#ifndef BOND_H
#define BOND_H

#include <glm/glm.hpp>
#include <memory>

class Atom; // Forward declaration to avoid circular dependency

/**
 * @brief Represents a chemical bond between two atoms.
 * 
 * This class stores information about the bond, including the two atoms it connects,
 * its type, and its associated energy.
 */
class Bond {
public:
    enum class Type {
        SINGLE,
        DOUBLE,
        TRIPLE,
        IONIC,
        METALLIC,
        HYDROGEN
    };

    /**
     * @brief Constructs a new Bond object.
     * 
     * @param atom1 A shared pointer to the first atom in the bond.
     * @param atom2 A shared pointer to the second atom in the bond.
     * @param type The type of the bond.
     * @param energy The energy associated with the bond (e.g., dissociation energy).
     */
    Bond(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2, Type type, float energy);

    /**
     * @brief Gets the first atom in the bond.
     * 
     * @return A shared pointer to the first atom.
     */
    std::shared_ptr<Atom> getAtom1() const { return m_atom1; }

    /**
     * @brief Gets the second atom in the bond.
     * 
     * @return A shared pointer to the second atom.
     */
    std::shared_ptr<Atom> getAtom2() const { return m_atom2; }

    /**
     * @brief Gets the type of the bond.
     * 
     * @return The bond type.
     */
    Type getType() const { return m_type; }

    /**
     * @brief Gets the energy of the bond.
     * 
     * @return The bond energy in eV.
     */
    float getEnergy() const { return m_energy; }

    /**
     * @brief Sets the energy of the bond.
     * 
     * @param energy The new bond energy in eV.
     */
    void setEnergy(float energy) { m_energy = energy; }

private:
    std::shared_ptr<Atom> m_atom1;
    std::shared_ptr<Atom> m_atom2;
    Type m_type;
    float m_energy; // in eV
};

#endif // BOND_H
