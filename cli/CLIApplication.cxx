#include "CLIApplication.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Constants.hxx"
#include <iostream>
#include <limits>
#include <sstream>

namespace NumberStore {
    CLIApplication::CLIApplication() : running(false) {
    }

    CLIApplication::~CLIApplication() {
        shutdown();
    }

    ErrorCode CLIApplication::initialize() {
        Logger::getInstance().setConsoleOutput(false); // Disable console logging for CLI
        
        std::cout << "Connecting to Number Store daemon..." << std::endl;
        
        ErrorCode result = client.connect();
        if (result != ErrorCode::SUCCESS) {
            displayError("Failed to connect to daemon. Is the daemon running?");
            return result;
        }
        
        running = true;
        std::cout << "Connected successfully!" << std::endl;
        return ErrorCode::SUCCESS;
    }

    void CLIApplication::run() {
        if (!running) {
            displayError("Application not initialized");
            return;
        }
        while (running && client.isConnected()) {
            showMainMenu();
            int choice = getUserChoice();
            processUserChoice(choice);
        }
        
        // Check if we exited due to connection failure
        if (running && !client.isConnected()) {
            std::cout << "\nConnection to daemon lost. Exiting..." << std::endl;
        }
    }

    void CLIApplication::shutdown() {
        if (running) {
            client.disconnect();
            running = false;
        }
    }

    void CLIApplication::showMainMenu() {
        std::cout << "\n" << std::string(40, '=') << "\n"
                << "Number Store - Main Menu\n"
                << std::string(40, '=') << "\n"
                << "1. Insert a number\n"
                << "2. Delete a number\n"
                << "3. Print all numbers\n"
                << "4. Delete all numbers\n"
                << "5. Exit\n"
                << std::string(40, '=') << std::endl;
    }

    int CLIApplication::getUserChoice() {
        int choice;
        while (true) {
            std::cout << "Enter your choice (1-5): ";
            
            if (std::cin >> choice) {
                // Clear the input buffer
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                
                if (choice >= 1 && choice <= 5) {
                    return choice;
                }
            } else {
                // Clear error flag and input buffer
                std::cin.clear();
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            }
            
            std::cout << "Invalid choice. Please enter a number between 1 and 5." << std::endl;
        }
    }

    void CLIApplication::processUserChoice(int choice) {
        switch (choice) {
            case 1:
                handleInsertNumber();
                break;
            case 2:
                handleDeleteNumber();
                break;
            case 3:
                handlePrintAllNumbers();
                break;
            case 4:
                handleDeleteAllNumbers();
                break;
            case 5:
                handleExit();
                break;
            default:
                displayError("Invalid choice");
                break;
        }
    }

    void CLIApplication::handleInsertNumber() {
        std::cout << "\n--- Insert Number ---" << std::endl;
        
        uint64_t number = getNumberInput("Enter a positive integer to insert: ");
        if (number <= 0) {
            return; // Error already displayed by getNumberInput
        }

        std::string result;
        ErrorCode error = client.insertNumber(number, result);
        
        if (error == ErrorCode::SUCCESS) {
            displayMessage(result);
        } else if (isConnectionError(error)) {
            handleConnectionError(result);
        } else {
            displayError(result);
        }
    }

    void CLIApplication::handleDeleteNumber() {
        std::cout << "\n--- Delete Number ---" << std::endl;

        uint64_t number = getNumberInput("Enter the number to delete: ");
        if (number <= 0) {
            return; // Error already displayed by getNumberInput
        }

        std::string result;
        ErrorCode error = client.deleteNumber(number, result);
        
        if (error == ErrorCode::SUCCESS) {
            displayMessage(result);
        } else if (isConnectionError(error)) {
            handleConnectionError(result);
        } else {
            displayError(result);
        }
    }

    void CLIApplication::handlePrintAllNumbers() {
        std::cout << "\n--- All Stored Numbers ---" << std::endl;
        
        std::string result;
        ErrorCode error = client.printAllNumbers(result);
        
        if (error == ErrorCode::SUCCESS) {
            if (result.empty() || result == "No numbers stored.") {
                std::cout << "No numbers are currently stored." << std::endl;
            } else {
                std::cout << "Number:Timestamp" << std::endl;
                std::cout << std::string(30, '-') << std::endl;
                std::cout << result << std::endl;
            }
        } else if (isConnectionError(error)) {
            handleConnectionError(result);
        } else {
            displayError(result);
        }
    }

    void CLIApplication::handleDeleteAllNumbers() {
        std::cout << "\n--- Delete All Numbers ---" << std::endl;
        
        if (!confirmAction("delete all stored numbers")) {
            std::cout << "Operation cancelled." << std::endl;
            return;
        }

        std::string result;
        ErrorCode error = client.deleteAllNumbers(result);
        
        if (error == ErrorCode::SUCCESS) {
            displayMessage(result);
        } else if (isConnectionError(error)) {
            handleConnectionError(result);
        } else {
            displayError(result);
        }
    }

    void CLIApplication::handleExit() {
        std::cout << "\n--- Exit ---" << std::endl;
        
        std::string result;
        client.exitSession(result);
        
        running = false;
        std::cout << Constants::MSG_GOODBYE << std::endl;
    }

    std::string CLIApplication::getUserInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    uint64_t CLIApplication::getNumberInput(const std::string& prompt) {
        while (true) {
            std::string input = getUserInput(prompt);
            
            uint64_t number;
            ErrorCode result = Validator::validateInsertInput(input, number);
            
            if (result == ErrorCode::SUCCESS) {
                return number;
            } else {
                displayError(ErrorHandler::getErrorMessage(result));
            }
        }
    }

    void CLIApplication::displayMessage(const std::string& message) {
        std::cout << message << std::endl;
    }

    void CLIApplication::displayError(const std::string& error) {
        std::cerr << error << std::endl;
    }

    bool CLIApplication::confirmAction(const std::string& action) {
        std::string input = getUserInput("Are you sure you want to " + action + "? (y/n): ");
        return !input.empty() && (input[0] == 'y' || input[0] == 'Y');
    }

    bool CLIApplication::isConnectionError(ErrorCode error) {
        return error == ErrorCode::CONNECTION_FAILED ||
               error == ErrorCode::PIPE_CONNECT_FAILED ||
               error == ErrorCode::READ_FAILED ||
               error == ErrorCode::WRITE_FAILED;
    }

    void CLIApplication::handleConnectionError(const std::string& message) {
        displayError("Connection lost: " + message);
        std::cout << "Daemon connection failed. Exiting CLI..." << std::endl;
        running = false;
    }
}
