#ifndef LOGGER_HXX
#define LOGGER_HXX

#include <string>
#include <mutex>
#include <fstream>
#include <memory>

namespace NumberStore {
    enum class LogLevel {
        INFO,
        WARNING,
        ERROR_LEVEL,
        DEBUG
    };

    class Logger {
    private:
        static std::unique_ptr<Logger> instance;
        static std::mutex logMutex;
        
        std::ofstream logFile;
        bool consoleOutput;
        LogLevel currentLevel;

        Logger();

    public:
        ~Logger();
        
        // Rule of 5: Explicitly delete copy and move operations for singleton
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;
        
        static Logger& getInstance();
        
        void log(LogLevel level, const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void debug(const std::string& message);
        
        void setConsoleOutput(bool enabled);
        void setLogLevel(LogLevel level);
        
    private:
        std::string levelToString(LogLevel level);
        std::string getCurrentTimeString();
        bool shouldLog(LogLevel level);
    };
}

#endif // LOGGER_HXX
