#ifndef DAEMON_SERVER_HXX
#define DAEMON_SERVER_HXX

#include "ConnectionManager.hxx"
#include "CommandProcessor.hxx"
#include "../storage/NumberStore.hxx"
#include "../utils/ErrorCodes.hxx"
#include <memory>
#include <thread>
#include <atomic>

namespace NumberStore {
    class DaemonServer {
    private:
        NumberStore numberStore;
        std::unique_ptr<CommandProcessor> processor;
        std::unique_ptr<ConnectionManager> connectionManager;
        std::unique_ptr<std::thread> serverThread;
        std::atomic<bool> running;

    public:
        DaemonServer();
        ~DaemonServer();

        DaemonServer(const DaemonServer&) = delete;
        DaemonServer& operator=(const DaemonServer&) = delete;
        DaemonServer(DaemonServer&&) = delete;
        DaemonServer& operator=(DaemonServer&&) = delete;

        ErrorCode start();
        void stop();
        void run();
        
        bool isRunning() const;
        size_t getActiveConnectionCount() const;
        
        // Access to number store for statistics
        const NumberStore& getNumberStore() const;
        
    };
}

#endif // DAEMON_SERVER_HXX
