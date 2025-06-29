#include "MathUtils.h"
#include <cmath>
#include <random>
#include <algorithm>

float MathUtils::distance(const glm::vec3& p1, const glm::vec3& p2) {
    return glm::length(p2 - p1);
}

float MathUtils::magnitude(const glm::vec3& v) {
    return glm::length(v);
}

glm::vec3 MathUtils::normalize(const glm::vec3& v) {
    float mag = magnitude(v);
    if (mag > 1e-9f) {
        return v / mag;
    }
    return glm::vec3(0.0f);
}

float MathUtils::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float MathUtils::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float MathUtils::joulesToEV(float joules) {
    return joules * JOULES_TO_EV;
}

float MathUtils::evToJoules(float eV) {
    return eV * EV_TO_JOULES;
}

float MathUtils::amuToKg(float amu) {
    return amu * AMU_TO_KG;
}

float MathUtils::kgToAmu(float kg) {
    return kg * KG_TO_AMU;
}

float MathUtils::calculateBindingEnergy(int A, int Z) {
    if (A <= 0 || Z < 0 || Z > A) return 0.0f;
    
    float a_v = 15.75f;  // Volume term coefficient (MeV)
    float a_s = 17.8f;   // Surface term coefficient (MeV)
    float a_c = 0.711f;  // Coulomb term coefficient (MeV)
    float a_A = 23.7f;   // Asymmetry term coefficient (MeV)
    float a_p = 11.18f;  // Pairing term coefficient (MeV)
    
    int N = A - Z; // Number of neutrons
    
    // Volume term
    float volume = a_v * A;
    
    // Surface term
    float surface = a_s * std::pow(A, 2.0f/3.0f);
    
    // Coulomb term
    float coulomb = a_c * (Z * Z) / std::pow(A, 1.0f/3.0f);
    
    // Asymmetry term
    float asymmetry = a_A * ((N - Z) * (N - Z)) / A;
    
    // Pairing term
    float pairing = 0.0f;
    if (Z % 2 == 0 && N % 2 == 0) {
        pairing = a_p / std::sqrt(A); // Even-even
    } else if (Z % 2 == 1 && N % 2 == 1) {
        pairing = -a_p / std::sqrt(A); // Odd-odd
    }
    // Even-odd and odd-even have pairing = 0
    
    float bindingEnergy = volume - surface - coulomb - asymmetry + pairing;
    
    return std::max(0.0f, bindingEnergy);
}

float MathUtils::randomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

int MathUtils::randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

