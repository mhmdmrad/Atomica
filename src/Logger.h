#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

/**
 * @brief Simple logging utility for the simulation.
 * 
 * This class provides logging functionality with different log levels
 * and output to both console and file.
 */
class Logger {
public:
    enum class Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    /**
     * @brief Gets the singleton instance of the Logger.
     * 
     * @return Reference to the Logger instance.
     */
    static Logger& getInstance();

    /**
     * @brief Sets the minimum log level.
     * 
     * @param level The minimum level to log.
     */
    void setLogLevel(Level level);

    /**
     * @brief Sets the log file path.
     * 
     * @param filename Path to the log file.
     * @return True if file was opened successfully.
     */
    bool setLogFile(const std::string& filename);

    /**
     * @brief Logs a debug message.
     * 
     * @param message The message to log.
     */
    void debug(const std::string& message);

    /**
     * @brief Logs an info message.
     * 
     * @param message The message to log.
     */
    void info(const std::string& message);

    /**
     * @brief Logs a warning message.
     * 
     * @param message The message to log.
     */
    void warning(const std::string& message);

    /**
     * @brief Logs an error message.
     * 
     * @param message The message to log.
     */
    void error(const std::string& message);

    /**
     * @brief Closes the log file.
     */
    void close();

private:
    Logger() : m_logLevel(Level::INFO) {}
    ~Logger() { close(); }

    Level m_logLevel;
    std::ofstream m_logFile;

    /**
     * @brief Internal logging function.
     * 
     * @param level The log level.
     * @param message The message to log.
     */
    void log(Level level, const std::string& message);

    /**
     * @brief Gets the current timestamp as a string.
     * 
     * @return Formatted timestamp string.
     */
    std::string getCurrentTimestamp();

    /**
     * @brief Converts log level to string.
     * 
     * @param level The log level.
     * @return String representation of the level.
     */
    std::string levelToString(Level level);
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)

#endif // LOGGER_H

