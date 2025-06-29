#include "Molecule.h"

void Molecule::addAtom(std::shared_ptr<Atom> atom) {
    m_atoms.push_back(atom);
}

void Molecule::addBond(std::shared_ptr<Bond> bond) {
    m_bonds.push_back(bond);
}
