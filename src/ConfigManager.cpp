#include "ConfigManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }

    m_config.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Find the '=' separator
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (!key.empty()) {
            m_config[key] = value;
        }
    }
    
    file.close();
    std::cout << "Loaded " << m_config.size() << " configuration entries from " << filename << std::endl;
    return true;
}

bool ConfigManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to create config file: " << filename << std::endl;
        return false;
    }

    file << "# Sandbox Simulation Configuration File" << std::endl;
    file << "# Generated automatically" << std::endl;
    file << std::endl;
    
    for (const auto& pair : m_config) {
        file << pair.first << "=" << pair.second << std::endl;
    }
    
    file.close();
    std::cout << "Saved " << m_config.size() << " configuration entries to " << filename << std::endl;
    return true;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) {
    auto it = m_config.find(key);
    return (it != m_config.end()) ? it->second : defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid integer value for key '" << key << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

float ConfigManager::getFloat(const std::string& key, float defaultValue) {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        try {
            return std::stof(it->second);
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid float value for key '" << key << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) {
    auto it = m_config.find(key);
    if (it != m_config.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        if (value == "true" || value == "1" || value == "yes" || value == "on") {
            return true;
        } else if (value == "false" || value == "0" || value == "no" || value == "off") {
            return false;
        } else {
            std::cerr << "Warning: Invalid boolean value for key '" << key << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    m_config[key] = value;
}

void ConfigManager::setInt(const std::string& key, int value) {
    m_config[key] = std::to_string(value);
}

void ConfigManager::setFloat(const std::string& key, float value) {
    m_config[key] = std::to_string(value);
}

void ConfigManager::setBool(const std::string& key, bool value) {
    m_config[key] = value ? "true" : "false";
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

