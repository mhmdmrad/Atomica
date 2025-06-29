#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <glm/glm.hpp>
#include <vector>

/**
 * @brief Utility class for mathematical operations used throughout the simulation.
 * 
 * This class provides common mathematical functions and constants
 * needed for physics calculations.
 */
class MathUtils {
public:
    // Physical constants
    static constexpr float ELEMENTARY_CHARGE = 1.602176634e-19f; // Coulombs
    static constexpr float ELECTRON_MASS = 9.1093837015e-31f;    // kg
    static constexpr float PROTON_MASS = 1.67262192369e-27f;     // kg
    static constexpr float NEUTRON_MASS = 1.67492749804e-27f;    // kg
    static constexpr float SPEED_OF_LIGHT = 299792458.0f;        // m/s
    static constexpr float PLANCK_CONSTANT = 6.62607015e-34f;    // J⋅s
    static constexpr float BOLTZMANN_CONSTANT = 1.380649e-23f;   // J/K
    static constexpr float AVOGADRO_NUMBER = 6.02214076e23f;     // mol^-1
    static constexpr float ATOMIC_MASS_UNIT = 1.66053906660e-27f; // kg
    
    // Conversion factors
    static constexpr float EV_TO_JOULES = 1.602176634e-19f;
    static constexpr float JOULES_TO_EV = 6.241509074e18f;
    static constexpr float AMU_TO_KG = 1.66053906660e-27f;
    static constexpr float KG_TO_AMU = 6.02214076e26f;

    /**
     * @brief Calculates the distance between two points.
     * 
     * @param p1 First point.
     * @param p2 Second point.
     * @return The distance between the points.
     */
    static float distance(const glm::vec3& p1, const glm::vec3& p2);

    /**
     * @brief Calculates the magnitude of a vector.
     * 
     * @param v The vector.
     * @return The magnitude of the vector.
     */
    static float magnitude(const glm::vec3& v);

    /**
     * @brief Normalizes a vector.
     * 
     * @param v The vector to normalize.
     * @return The normalized vector.
     */
    static glm::vec3 normalize(const glm::vec3& v);

    /**
     * @brief Clamps a value between min and max.
     * 
     * @param value The value to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @return The clamped value.
     */
    static float clamp(float value, float min, float max);

    /**
     * @brief Linear interpolation between two values.
     * 
     * @param a First value.
     * @param b Second value.
     * @param t Interpolation factor (0-1).
     * @return The interpolated value.
     */
    static float lerp(float a, float b, float t);

    /**
     * @brief Converts energy from Joules to electron volts.
     * 
     * @param joules Energy in Joules.
     * @return Energy in electron volts.
     */
    static float joulesToEV(float joules);

    /**
     * @brief Converts energy from electron volts to Joules.
     * 
     * @param eV Energy in electron volts.
     * @return Energy in Joules.
     */
    static float evToJoules(float eV);

    /**
     * @brief Converts mass from atomic mass units to kilograms.
     * 
     * @param amu Mass in atomic mass units.
     * @return Mass in kilograms.
     */
    static float amuToKg(float amu);

    /**
     * @brief Converts mass from kilograms to atomic mass units.
     * 
     * @param kg Mass in kilograms.
     * @return Mass in atomic mass units.
     */
    static float kgToAmu(float kg);

    /**
     * @brief Calculates the binding energy using the semi-empirical mass formula.
     * 
     * @param A Mass number.
     * @param Z Atomic number.
     * @return Binding energy in MeV.
     */
    static float calculateBindingEnergy(int A, int Z);

    /**
     * @brief Generates a random float between min and max.
     * 
     * @param min Minimum value.
     * @param max Maximum value.
     * @return Random float.
     */
    static float randomFloat(float min, float max);

    /**
     * @brief Generates a random integer between min and max (inclusive).
     * 
     * @param min Minimum value.
     * @param max Maximum value.
     * @return Random integer.
     */
    static int randomInt(int min, int max);
};

#endif // MATH_UTILS_H
