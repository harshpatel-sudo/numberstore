#ifndef SINGLEINSTANCEMANAGER_HXX
#define SINGLEINSTANCEMANAGER_HXX

#include <windows.h>
#include <string>
#include "ErrorCodes.hxx"

namespace NumberStore {
    class SingleInstanceManager {
    private:
        HANDLE mutexHandle;
        std::string mutexName;
        bool isOwner;

        SingleInstanceManager(const SingleInstanceManager&) = delete;
        SingleInstanceManager& operator=(const SingleInstanceManager&) = delete;
        SingleInstanceManager(SingleInstanceManager&&) = delete;
        SingleInstanceManager& operator=(SingleInstanceManager&&) = delete;

    public:
        explicit SingleInstanceManager(const std::string& appName);
        ~SingleInstanceManager();

        ErrorCode tryLock();
        void unlock();
        bool isLocked() const;
    };
}

#endif // SINGLEINSTANCEMANAGER_HXX
