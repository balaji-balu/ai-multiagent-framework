// logging_service.h
#ifndef AI_FRAMEWORK_LOGGING_SERVICE_H
#define AI_FRAMEWORK_LOGGING_SERVICE_H

#include <string>
#include <mutex>
#include <fstream>
#include <memory>

namespace ai_framework {

/**
 * @brief Log level enumeration
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 * @brief Thread-safe logging service
 */
class LoggingService {
public:
    /**
     * @brief Get the singleton instance of LoggingService
     * 
     * @return LoggingService& Reference to the LoggingService instance
     */
    static LoggingService& GetInstance();
    
    /**
     * @brief Initialize the logging service
     * 
     * @param logFile Path to the log file
     * @param logLevel Minimum log level to output
     * @param logToConsole Whether to also log to console
     * @return bool True if initialization succeeded, false otherwise
     */
    bool Initialize(
        const std::string& logFile, 
        LogLevel logLevel = LogLevel::INFO,
        bool logToConsole = true);
    
    /**
     * @brief Set the log level
     * 
     * @param level New log level
     */
    void SetLogLevel(LogLevel level);
    
    /**
     * @brief Log a message
     * 
     * @param level Log level of the message
     * @param message Message to log
     */
    void Log(LogLevel level, const std::string& message);
    
    /**
     * @brief Close the log file
     */
    void Close();

private:
    /**
     * @brief Private constructor for singleton
     */
    LoggingService() = default;
    
    /**
     * @brief Private copy constructor (not implemented)
     */
    LoggingService(const LoggingService&) = delete;
    
    /**
     * @brief Private assignment operator (not implemented)
     */
    LoggingService& operator=(const LoggingService&) = delete;
    
    /**
     * @brief Convert a log level to a string
     * 
     * @param level Log level to convert
     * @return std::string String representation of the log level
     */
    std::string LogLevelToString(LogLevel level);
    
    /** Log file stream */
    std::unique_ptr<std::ofstream> m_logFile;
    
    /** Minimum log level to output */
    LogLevel m_logLevel = LogLevel::INFO;
    
    /** Whether to also log to console */
    bool m_logToConsole = true;
    
    /** Mutex for thread-safe logging */
    std::mutex m_mutex;
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_LOGGING_SERVICE_H