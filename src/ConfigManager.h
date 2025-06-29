#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <unordered_map>
#include <fstream>

/**
 * @brief Manages configuration settings for the simulation.
 * 
 * This class handles loading and saving configuration parameters
 * from/to a configuration file.
 */
class ConfigManager {
public:
    /**
     * @brief Gets the singleton instance of the ConfigManager.
     * 
     * @return Reference to the ConfigManager instance.
     */
    static ConfigManager& getInstance();

    /**
     * @brief Loads configuration from a file.
     * 
     * @param filename Path to the configuration file.
     * @return True if loaded successfully.
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Saves configuration to a file.
     * 
     * @param filename Path to the configuration file.
     * @return True if saved successfully.
     */
    bool saveToFile(const std::string& filename);

    /**
     * @brief Gets a string value from configuration.
     * 
     * @param key The configuration key.
     * @param defaultValue Default value if key not found.
     * @return The configuration value.
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Gets an integer value from configuration.
     * 
     * @param key The configuration key.
     * @param defaultValue Default value if key not found.
     * @return The configuration value.
     */
    int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief Gets a float value from configuration.
     * 
     * @param key The configuration key.
     * @param defaultValue Default value if key not found.
     * @return The configuration value.
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief Gets a boolean value from configuration.
     * 
     * @param key The configuration key.
     * @param defaultValue Default value if key not found.
     * @return The configuration value.
     */
    bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * @brief Sets a string value in configuration.
     * 
     * @param key The configuration key.
     * @param value The value to set.
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * @brief Sets an integer value in configuration.
     * 
     * @param key The configuration key.
     * @param value The value to set.
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief Sets a float value in configuration.
     * 
     * @param key The configuration key.
     * @param value The value to set.
     */
    void setFloat(const std::string& key, float value);

    /**
     * @brief Sets a boolean value in configuration.
     * 
     * @param key The configuration key.
     * @param value The value to set.
     */
    void setBool(const std::string& key, bool value);

private:
    ConfigManager() = default;
    ~ConfigManager() = default;

    std::unordered_map<std::string, std::string> m_config;

    /**
     * @brief Trims whitespace from a string.
     * 
     * @param str The string to trim.
     * @return The trimmed string.
     */
    std::string trim(const std::string& str);
};

#endif // CONFIG_MANAGER_H

