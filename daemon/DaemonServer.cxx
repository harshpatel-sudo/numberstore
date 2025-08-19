#include "DaemonServer.hxx"
#include "SignalHandler.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"

namespace NumberStore {
    DaemonServer::DaemonServer() : running(false) {
        processor = std::make_unique<CommandProcessor>(numberStore);
        connectionManager = std::make_unique<ConnectionManager>(*processor);
    }

    DaemonServer::~DaemonServer() {
        stop();
    }

    ErrorCode DaemonServer::start() {
        if (running.load()) {
            Logger::getInstance().warning("Daemon server is already running");
            return ErrorCode::SUCCESS;
        }

        Logger::getInstance().info("Starting daemon server");
        
        Config& config = Config::getInstance();
        ErrorCode result = connectionManager->start(config.getPipeName());
        
        if (result != ErrorCode::SUCCESS) {
            Logger::getInstance().error("Failed to start connection manager");
            return result;
        }

        running.store(true);
        Logger::getInstance().info("Daemon server started successfully");
        return ErrorCode::SUCCESS;
    }

    void DaemonServer::stop() {
        if (!running.load()) {
            return;
        }

        Logger::getInstance().info("Stopping daemon server");
        running.store(false);
        
        if (connectionManager) {
            connectionManager->stop();
        }
        
        if (serverThread && serverThread->joinable()) {
            serverThread->join();
        }
        
        Logger::getInstance().info("Daemon server stopped");
        std::exit(0);
    }

    void DaemonServer::run() {
        if (!running.load()) {
            Logger::getInstance().error("Cannot run daemon server - not started");
            return;
        }

        Logger::getInstance().info("Daemon server entering main loop");
        
        // Run the connection manager in the main thread
        if (connectionManager) {
            connectionManager->run();
        }
        
        Logger::getInstance().info("Daemon server main loop finished");
    }

    bool DaemonServer::isRunning() const {
        return running.load();
    }

    size_t DaemonServer::getActiveConnectionCount() const {
        if (connectionManager) {
            return connectionManager->getActiveConnectionCount();
        }
        return 0;
    }

    const NumberStore& DaemonServer::getNumberStore() const {
        return numberStore;
    }
}
