#include "Bond.h"

Bond::Bond(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2, Type type, float energy)
    : m_atom1(atom1),
      m_atom2(atom2),
      m_type(type),
      m_energy(energy) {}
