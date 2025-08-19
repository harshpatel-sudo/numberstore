#include "SingleInstanceManager.hxx"
#include "Logger.hxx"

namespace NumberStore {
    SingleInstanceManager::SingleInstanceManager(const std::string& appName) 
        : mutexHandle(nullptr), isOwner(false) {
        mutexName = "Global\\NumberStore_" + appName + "_Mutex";
    }

    SingleInstanceManager::~SingleInstanceManager() {
        unlock();
    }

    ErrorCode SingleInstanceManager::tryLock() {
        if (mutexHandle != nullptr) {
            return ErrorCode::SUCCESS; // Already locked
        }

        // Create a named mutex
        mutexHandle = CreateMutexA(
            nullptr,    // Default security attributes
            TRUE,       // Initially owned by calling thread
            mutexName.c_str()
        );

        if (mutexHandle == nullptr) {
            DWORD error = GetLastError();
            Logger::getInstance().error("Failed to create mutex: " + std::to_string(error));
            return ErrorCode::INITIALIZATION_FAILED;
        }

        DWORD waitResult = GetLastError();
        if (waitResult == ERROR_ALREADY_EXISTS) {
            // Another instance is already running
            Logger::getInstance().warning("Another daemon instance is already running");
            CloseHandle(mutexHandle);
            mutexHandle = nullptr;
            return ErrorCode::INSTANCE_ALREADY_RUNNING;
        }

        // We successfully created and own the mutex
        isOwner = true;
        Logger::getInstance().info("Single instance lock acquired");
        return ErrorCode::SUCCESS;
    }

    void SingleInstanceManager::unlock() {
        if (mutexHandle != nullptr && isOwner) {
            ReleaseMutex(mutexHandle);
            CloseHandle(mutexHandle);
            mutexHandle = nullptr;
            isOwner = false;
            Logger::getInstance().info("Single instance lock released");
        }
    }

    bool SingleInstanceManager::isLocked() const {
        return mutexHandle != nullptr && isOwner;
    }
}
