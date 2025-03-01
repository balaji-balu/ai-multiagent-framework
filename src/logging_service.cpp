// logging_service.cpp
#include "logging_service.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace ai_framework {

LoggingService& LoggingService::GetInstance() {
    static LoggingService instance;
    return instance;
}

bool LoggingService::Initialize(
    const std::string& logFile, 
    LogLevel logLevel,
    bool logToConsole) {
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_logLevel = logLevel;
    m_logToConsole = logToConsole;
    
    if (!logFile.empty()) {
        m_logFile = std::make_unique<std::ofstream>(logFile, std::ios::app);
        if (!m_logFile->is_open()) {
            if (m_logToConsole) {
                std::cerr << "Failed to open log file: " << logFile << std::endl;
            }
            m_logFile.reset();
            return false;
        }
    }
    
    Log(LogLevel::INFO, "Logging service initialized");
    return true;
}

void LoggingService::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logLevel = level;
    
    Log(LogLevel::INFO, "Log level set to " + LogLevelToString(level));
}

void LoggingService::Log(LogLevel level, const std::string& message) {
    if (level < m_logLevel) {
        return;
    }
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    // Format time string
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Format log message
    std::string formattedMessage = 
        ss.str() + " [" + LogLevelToString(level) + "] " + message;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Write to file if available
    if (m_logFile && m_logFile->is_open()) {
        *m_logFile << formattedMessage << std::endl;
        m_logFile->flush();
    }
    
    // Write to console if enabled
    if (m_logToConsole) {
        std::ostream& out = (level >= LogLevel::WARNING) ? std::cerr : std::cout;
        out << formattedMessage << std::endl;
    }
}

void LoggingService::Close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_logFile && m_logFile->is_open()) {
        m_logFile->close();
    }
    
    m_logFile.reset();
}

std::string LoggingService::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

} // namespace ai_framework