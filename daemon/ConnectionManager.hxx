#ifndef CONNECTION_MANAGER_HXX
#define CONNECTION_MANAGER_HXX

#include "../ipc/NamedPipeServer.hxx"
#include "ClientHandler.hxx"
#include "CommandProcessor.hxx"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

namespace NumberStore {
    class ConnectionManager {
    private:
        NamedPipeServer server;
        CommandProcessor& processor;
        std::vector<std::unique_ptr<std::thread>> clientThreads;
        std::vector<std::unique_ptr<ClientHandler>> clientHandlers;
        mutable std::mutex connectionsMutex;
        std::atomic<bool> running;

    public:
        explicit ConnectionManager(CommandProcessor& proc);
        ~ConnectionManager();

        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;
        ConnectionManager(ConnectionManager&&) = delete;
        ConnectionManager& operator=(ConnectionManager&&) = delete;

        ErrorCode start(const std::string& pipeName);
        void stop();
        void run();
        
        size_t getActiveConnectionCount() const;
        bool isRunning() const;
        
    private:
        void acceptConnections();
        void cleanupFinishedClients();
        void stopAllClients();
        void waitForAllClients();
    };
}

#endif // CONNECTION_MANAGER_HXX
