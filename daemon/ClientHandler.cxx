#include "ClientHandler.hxx"
#include "SignalHandler.hxx"
#include "../utils/Logger.hxx"
#include <sstream>
#include <chrono>

namespace NumberStore {
    ClientHandler::ClientHandler(std::unique_ptr<NamedPipeConnection> conn, CommandProcessor& proc)
        : connection(std::move(conn)), processor(proc), active(true) {
        clientId = generateClientId();
    }

    ClientHandler::~ClientHandler() {
        cleanup();
    }

    void ClientHandler::run() {
        Logger::getInstance().info("Client handler started for client: " + clientId);
        
        try {
            while (active.load() && connection->isConnected() && !SignalHandler::isShutdownRequested()) {
                if (!handleSingleCommand()) {
                    break;
                }
            }
        }
        catch (const std::exception& e) {
            Logger::getInstance().error("Exception in client handler: " + std::string(e.what()));
        }
        
        cleanup();
        Logger::getInstance().info("Client handler finished for client: " + clientId);
    }

    void ClientHandler::stop() {
        active.store(false);
        Logger::getInstance().debug("Stop requested for client: " + clientId);
    }

    bool ClientHandler::isActive() const {
        return active.load() && connection && connection->isConnected();
    }

    const std::string& ClientHandler::getClientId() const {
        return clientId;
    }

    bool ClientHandler::handleSingleCommand() {
        // Read command from client
        std::string rawMessage;
        ErrorCode readResult = connection->read(rawMessage);
        
        if (readResult != ErrorCode::SUCCESS) {
            if (readResult == ErrorCode::CONNECTION_FAILED) {
                Logger::getInstance().info("Client disconnected: " + clientId);
            } else {
                Logger::getInstance().error("Failed to read from client " + clientId + ": " + 
                                         ErrorHandler::getErrorMessage(readResult));
            }
            return false;
        }

        // Deserialize command
        auto command = MessageSerializer::deserializeCommand(rawMessage);
        if (!command) {
            Logger::getInstance().error("Failed to deserialize command from client: " + clientId);
            
            // Send error response
            auto errorResponse = Response::createErrorResponse(ErrorCode::SERIALIZATION_ERROR, "");
            std::string errorMessage = MessageSerializer::serializeResponse(*errorResponse);
            connection->write(errorMessage);
            return true; // Continue processing other commands
        }

        // Check for exit command
        if (command->getCommandType() == CommandType::EXIT) {
            // Process exit command and send response
            auto response = processor.processCommand(*command);
            std::string responseMessage = MessageSerializer::serializeResponse(*response);
            connection->write(responseMessage);
            
            Logger::getInstance().info("Client " + clientId + " requested exit");
            return false; // End this client session
        }

        // Process command
        auto response = processor.processCommand(*command);
        
        // Send response back to client
        std::string responseMessage = MessageSerializer::serializeResponse(*response);
        ErrorCode writeResult = connection->write(responseMessage);
        
        if (writeResult != ErrorCode::SUCCESS) {
            Logger::getInstance().error("Failed to write response to client " + clientId + ": " + 
                                     ErrorHandler::getErrorMessage(writeResult));
            return false;
        }

        return true; // Continue processing commands
    }

    void ClientHandler::cleanup() {
        active.store(false);
        
        if (connection && connection->isConnected()) {
            connection->disconnect();
        }
    }

    std::string ClientHandler::generateClientId() {
        static std::atomic<int> counter{0};
        
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        
        std::ostringstream oss;
        oss << "CLIENT-" << timestamp << "-" << counter.fetch_add(1);
        return oss.str();
    }
}
