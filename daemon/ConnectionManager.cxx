#include "ConnectionManager.hxx"
#include "SignalHandler.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"
#include <algorithm>

namespace NumberStore {
    ConnectionManager::ConnectionManager(CommandProcessor& proc) 
        : processor(proc), running(false) {
    }

    ConnectionManager::~ConnectionManager() {
        stop();
    }

    ErrorCode ConnectionManager::start(const std::string& pipeName) {
        if (running.load()) {
            return ErrorCode::SUCCESS;
        }

        ErrorCode result = server.start(pipeName);
        if (result != ErrorCode::SUCCESS) {
            Logger::getInstance().error("Failed to start named pipe server");
            return result;
        }

        running.store(true);
        Logger::getInstance().info("Connection manager started");
        return ErrorCode::SUCCESS;
    }

    void ConnectionManager::stop() {
        if (!running.load()) {
            return;
        }

        Logger::getInstance().info("Stopping connection manager");
        running.store(false);
        
        stopAllClients();
        server.stop();
        
        Logger::getInstance().info("Connection manager stopped");
    }

    void ConnectionManager::run() {
        Logger::getInstance().info("Connection manager main loop started");
        
        while (running.load() && !SignalHandler::isShutdownRequested()) {
            acceptConnections();
            cleanupFinishedClients();
            
            // Small delay to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        Logger::getInstance().info("Connection manager main loop finished");
    }

    size_t ConnectionManager::getActiveConnectionCount() const {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        return std::count_if(clientHandlers.begin(), clientHandlers.end(),
                           [](const auto& handler) { return handler->isActive(); });
    }

    bool ConnectionManager::isRunning() const {
        return running.load();
    }

    void ConnectionManager::acceptConnections() {
        Config& config = Config::getInstance();
        
        // Check if we've reached the maximum number of connections
        if (getActiveConnectionCount() >= config.getMaxConnections()) {
            return;
        }

        // Try to accept a new connection (non-blocking)
        auto connection = server.acceptConnection();
        if (!connection) {
            return; // No new connection available
        }

        // Create client handler and thread
        auto handler = std::make_unique<ClientHandler>(std::move(connection), processor);
        auto thread = std::make_unique<std::thread>(&ClientHandler::run, handler.get());

        // Add to our collections
        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            clientHandlers.push_back(std::move(handler));
            clientThreads.push_back(std::move(thread));
        }
        
        Logger::getInstance().info("New client connected. Active connections: " + 
                                 std::to_string(getActiveConnectionCount()));
    }

    void ConnectionManager::cleanupFinishedClients() {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        
        // Find finished threads and clean them up
        auto threadIt = clientThreads.begin();
        auto handlerIt = clientHandlers.begin();
        
        while (threadIt != clientThreads.end() && handlerIt != clientHandlers.end()) {
            if (!(*handlerIt)->isActive()) {
                // Client is finished, clean up
                if ((*threadIt)->joinable()) {
                    (*threadIt)->join();
                }
                
                Logger::getInstance().debug("Cleaned up finished client: " + (*handlerIt)->getClientId());
                
                threadIt = clientThreads.erase(threadIt);
                handlerIt = clientHandlers.erase(handlerIt);
            } else {
                ++threadIt;
                ++handlerIt;
            }
        }
    }

    void ConnectionManager::stopAllClients() {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        
        Logger::getInstance().info("Stopping all client handlers (" + 
                                 std::to_string(clientHandlers.size()) + " clients)");
        
        for (auto& handler : clientHandlers) {
            handler->stop();
        }
    }
}
