#ifndef CLI_APPLICATION_HXX
#define CLI_APPLICATION_HXX

#include "DaemonClient.hxx"
#include "../utils/ErrorCodes.hxx"
#include "../utils/Validator.hxx"
#include <string>

namespace NumberStore {
    class CLIApplication {
    private:
        DaemonClient client;
        bool running;

    public:
        CLIApplication();
        ~CLIApplication();

        CLIApplication(const CLIApplication&) = delete;
        CLIApplication& operator=(const CLIApplication&) = delete;
        CLIApplication(CLIApplication&&) = delete;
        CLIApplication& operator=(CLIApplication&&) = delete;

        ErrorCode initialize();
        void run();
        void shutdown();
        
    private:
        void showMainMenu();
        int getUserChoice();
        void processUserChoice(int choice);
        
        void handleInsertNumber();
        void handleDeleteNumber();
        void handlePrintAllNumbers();
        void handleDeleteAllNumbers();
        
        bool isConnectionError(ErrorCode error);
        void handleConnectionError(const std::string& message);
        void handleExit();
        
        std::string getUserInput(const std::string& prompt);
        uint64_t getNumberInput(const std::string& prompt);
        void displayMessage(const std::string& message);
        void displayError(const std::string& error);
        
        bool confirmAction(const std::string& action);
    };
}

#endif // CLI_APPLICATION_HXX
