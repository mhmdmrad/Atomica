#include "Atom.h"
#include "Particle.h"
#include <algorithm>

Atom::Atom(int atomicNumber, int massNumber, const glm::vec3& position)
    : m_atomicNumber(atomicNumber),
      m_massNumber(massNumber) {
    // Create nucleus (simplified mass and charge for now)
    float nucleusMass = (atomicNumber * 1.672e-27f) + ((massNumber - atomicNumber) * 1.674e-27f); // Protons + Neutrons
    float nucleusCharge = atomicNumber * 1.602e-19f; // Protons
    m_nucleus = std::make_shared<Nucleus>(atomicNumber, massNumber, position, glm::vec3(0.0f), nucleusMass, nucleusCharge);

    // Add initial electrons (neutral atom)
    for (int i = 0; i < atomicNumber; ++i) {
        // Initial orbital level can be 1 for simplicity, or more complex based on electron configuration rules
        m_electrons.push_back(std::make_shared<Electron>(position, glm::vec3(0.0f), 1));
    }
}

Atom::~Atom() {
    // Destructor, shared_ptr will handle memory deallocation
}

const glm::vec3& Atom::getPosition() const {
    return m_nucleus->getPosition();
}

void Atom::setPosition(const glm::vec3& position) {
    glm::vec3 delta = position - m_nucleus->getPosition();
    m_nucleus->setPosition(position);
    for (auto& electron : m_electrons) {
        electron->setPosition(electron->getPosition() + delta);
    }
}

void Atom::addElectron(std::shared_ptr<Electron> electron) {
    m_electrons.push_back(electron);
}

bool Atom::removeElectron(std::shared_ptr<Electron> electron) {
    auto it = std::remove(m_electrons.begin(), m_electrons.end(), electron);
    if (it != m_electrons.end()) {
        m_electrons.erase(it, m_electrons.end());
        return true;
    }
    return false;
}


