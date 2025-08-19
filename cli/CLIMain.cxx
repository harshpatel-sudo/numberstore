#include "CLIApplication.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"
#include <iostream>

int main() {
    try {
        // Initialize configuration
        NumberStore::Config& config = NumberStore::Config::getInstance();
        config.loadDefaults();
        
        // Create and run the CLI application
        NumberStore::CLIApplication app;
        
        NumberStore::ErrorCode initResult = app.initialize();
        if (initResult != NumberStore::ErrorCode::SUCCESS) {
            std::cerr << "Failed to initialize CLI application: " << 
                        NumberStore::ErrorHandler::getErrorMessage(initResult) << std::endl;
            std::cerr << "Make sure the daemon is running and try again." << std::endl;
            return 1;
        }
        
        // Run the main application loop
        app.run();
        
        // Cleanup
        app.shutdown();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
