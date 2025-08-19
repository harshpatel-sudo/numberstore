#include "DaemonServer.hxx"
#include "SignalHandler.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"
#include "../utils/SingleInstanceManager.hxx"
#include <iostream>
#include <cstdlib>

int main() {
    try {
        // Initialize configuration
        NumberStore::Config& config = NumberStore::Config::getInstance();
        config.loadDefaults();
        
        // Set up logging
        NumberStore::Logger& logger = NumberStore::Logger::getInstance();
        logger.setConsoleOutput(true);
        logger.info("Number Store Daemon starting up...");
        
        // Check for single instance
        NumberStore::SingleInstanceManager instanceManager("Daemon");
        NumberStore::ErrorCode lockResult = instanceManager.tryLock();
        
        if (lockResult == NumberStore::ErrorCode::INSTANCE_ALREADY_RUNNING) {
            std::cerr << "Error: Another daemon instance is already running." << std::endl;
            std::cerr << "Please stop the existing daemon before starting a new one." << std::endl;
            logger.error("Daemon startup failed: Another instance already running");
            return 1;
        }
        
        if (NumberStore::ErrorHandler::isError(lockResult)) {
            std::cerr << "Error: Failed to acquire single instance lock: " 
                      << NumberStore::ErrorHandler::getErrorMessage(lockResult) << std::endl;
            logger.error("Failed to acquire single instance lock");
            return 1;
        }
        
        // Set up signal handlers for graceful shutdown
        NumberStore::SignalHandler::setupSignalHandlers();
        
        // Create and start the daemon server
        NumberStore::DaemonServer server;
        NumberStore::SignalHandler::registerServer(&server);
        
        NumberStore::ErrorCode startResult = server.start();
        if (startResult != NumberStore::ErrorCode::SUCCESS) {
            logger.error("Failed to start daemon server: " + 
                        NumberStore::ErrorHandler::getErrorMessage(startResult));
            instanceManager.unlock(); // Release the lock before exiting
            return 1;
        }
        
        logger.info("Daemon server started. Waiting for client connections...");
        logger.info("Press Ctrl+C to stop the daemon.");
        
        // Run the main server loop
        server.run();
        
        // Check if shutdown was requested
        if (NumberStore::SignalHandler::isShutdownRequested()) {
            logger.info("Shutdown signal detected, stopping server...");
        }
        
        // Cleanup
        logger.info("Main loop exited, checking if server needs to be stopped...");
        if (server.isRunning()) {
            logger.info("Shutting down daemon server...");
            server.stop();
            logger.info("Server stop completed");
        } else {
            logger.info("Server already stopped");
        }
        
        NumberStore::SignalHandler::cleanup();
        logger.info("Signal handler cleanup completed");
        
        // Release the single instance lock
        instanceManager.unlock();
        logger.info("Single instance lock released");
        
        logger.info("Daemon server shutdown complete.");
        
        // Force exit to ensure all threads are terminated
        std::exit(0);
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        NumberStore::Logger::getInstance().error("Fatal error: " + std::string(e.what()));
        // Note: instanceManager destructor will handle cleanup
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        NumberStore::Logger::getInstance().error("Unknown fatal error occurred");
        // Note: instanceManager destructor will handle cleanup
        return 1;
    }
}
