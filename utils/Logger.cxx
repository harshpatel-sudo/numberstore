#include "Logger.hxx"
#include "TimeUtils.hxx"
#include <iostream>
#include <sstream>

namespace NumberStore {
    std::unique_ptr<Logger> Logger::instance = nullptr;
    std::mutex Logger::logMutex;

    Logger::Logger() : consoleOutput(true), currentLevel(LogLevel::INFO) {
        // logFile.open("numberstore.log", std::ios::app);
    }

    Logger::~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    Logger& Logger::getInstance() {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!instance) {
            instance = std::unique_ptr<Logger>(new Logger());
        }
        return *instance;
    }

    void Logger::log(LogLevel level, const std::string& message) {
        if (!shouldLog(level)) return;

        std::lock_guard<std::mutex> lock(logMutex);
        
        std::ostringstream logEntry;
        logEntry << "[" << getCurrentTimeString() << "] "
                 << "[" << levelToString(level) << "] "
                 << message;

        std::string logLine = logEntry.str();

        if (consoleOutput) {
            std::cout << logLine << std::endl;
        }

        if (logFile.is_open()) {
            logFile << logLine << std::endl;
            logFile.flush();
        }
    }

    void Logger::info(const std::string& message) {
        log(LogLevel::INFO, message);
    }

    void Logger::warning(const std::string& message) {
        log(LogLevel::WARNING, message);
    }

    void Logger::error(const std::string& message) {
        log(LogLevel::ERROR_LEVEL, message);
    }

    void Logger::debug(const std::string& message) {
        log(LogLevel::DEBUG, message);
    }

    void Logger::setConsoleOutput(bool enabled) {
        std::lock_guard<std::mutex> lock(logMutex);
        consoleOutput = enabled;
    }

    void Logger::setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(logMutex);
        currentLevel = level;
    }

    std::string Logger::levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR_LEVEL: return "ERROR";
            case LogLevel::DEBUG: return "DEBUG";
            default: return "UNKNOWN";
        }
    }

    std::string Logger::getCurrentTimeString() {
        return TimeUtils::getCurrentTimestampString();
    }

    bool Logger::shouldLog(LogLevel level) {
        return static_cast<int>(level) >= static_cast<int>(currentLevel);
    }
}
