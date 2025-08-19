#include "DaemonClient.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"

namespace NumberStore {
    DaemonClient::DaemonClient() : connected(false) {
        client = std::make_unique<NamedPipeClient>();
    }

    DaemonClient::~DaemonClient() {
        disconnect();
    }

    ErrorCode DaemonClient::connect() {
        if (connected) {
            return ErrorCode::SUCCESS;
        }

        Config& config = Config::getInstance();
        ErrorCode result = client->connect(config.getPipeName());
        
        if (result == ErrorCode::SUCCESS) {
            connected = true;
            Logger::getInstance().info("Connected to daemon");
        } else {
            Logger::getInstance().error("Failed to connect to daemon: " + 
                                     ErrorHandler::getErrorMessage(result));
        }

        return result;
    }

    ErrorCode DaemonClient::disconnect() {
        if (!connected) {
            return ErrorCode::SUCCESS;
        }

        ErrorCode result = client->disconnect();
        connected = false;
        
        Logger::getInstance().info("Disconnected from daemon");
        return result;
    }

    ErrorCode DaemonClient::sendCommand(const Command& command, std::unique_ptr<Response>& response) {
        if (!connected) {
            return ErrorCode::CONNECTION_FAILED;
        }

        return sendCommandInternal(command, response);
    }

    ErrorCode DaemonClient::insertNumber(uint64_t number, std::string& result) {
        auto command = Command::createInsertCommand(number);
        std::unique_ptr<Response> response;
        
        ErrorCode error = sendCommand(*command, response);
        if (error != ErrorCode::SUCCESS) {
            result = "Failed to communicate with daemon: " + ErrorHandler::getErrorMessage(error);
            return error;
        }

        result = formatResponse(*response);
        return response->isSuccess() ? ErrorCode::SUCCESS : response->getErrorCode();
    }

    ErrorCode DaemonClient::deleteNumber(uint64_t number, std::string& result) {
        auto command = Command::createDeleteCommand(number);
        std::unique_ptr<Response> response;
        
        ErrorCode error = sendCommand(*command, response);
        if (error != ErrorCode::SUCCESS) {
            result = "Failed to communicate with daemon: " + ErrorHandler::getErrorMessage(error);
            return error;
        }

        result = formatResponse(*response);
        return response->isSuccess() ? ErrorCode::SUCCESS : response->getErrorCode();
    }

    ErrorCode DaemonClient::printAllNumbers(std::string& result) {
        auto command = Command::createPrintAllCommand();
        std::unique_ptr<Response> response;
        
        ErrorCode error = sendCommand(*command, response);
        if (error != ErrorCode::SUCCESS) {
            result = "Failed to communicate with daemon: " + ErrorHandler::getErrorMessage(error);
            return error;
        }

        if (response->getResponseType() == ResponseType::DATA) {
            result = response->getData();
        } else {
            result = formatResponse(*response);
        }
        
        return response->isSuccess() ? ErrorCode::SUCCESS : response->getErrorCode();
    }

    ErrorCode DaemonClient::deleteAllNumbers(std::string& result) {
        auto command = Command::createDeleteAllCommand();
        std::unique_ptr<Response> response;
        
        ErrorCode error = sendCommand(*command, response);
        if (error != ErrorCode::SUCCESS) {
            result = "Failed to communicate with daemon: " + ErrorHandler::getErrorMessage(error);
            return error;
        }

        result = formatResponse(*response);
        return response->isSuccess() ? ErrorCode::SUCCESS : response->getErrorCode();
    }

    ErrorCode DaemonClient::exitSession(std::string& result) {
        auto command = Command::createExitCommand();
        std::unique_ptr<Response> response;
        
        ErrorCode error = sendCommand(*command, response);
        if (error != ErrorCode::SUCCESS) {
            result = "Failed to communicate with daemon: " + ErrorHandler::getErrorMessage(error);
            return error;
        }

        result = formatResponse(*response);
        return response->isSuccess() ? ErrorCode::SUCCESS : response->getErrorCode();
    }

    bool DaemonClient::isConnected() const {
        return connected && client && client->isConnected();
    }

    ErrorCode DaemonClient::sendCommandInternal(const Command& command, std::unique_ptr<Response>& response) {
        // Serialize and send command
        std::string serializedCommand = MessageSerializer::serializeCommand(command);
        ErrorCode sendResult = client->sendMessage(serializedCommand);
        
        if (sendResult != ErrorCode::SUCCESS) {
            Logger::getInstance().error("Failed to send command to daemon");
            return sendResult;
        }

        // Receive and deserialize response
        std::string serializedResponse;
        ErrorCode receiveResult = client->receiveMessage(serializedResponse);
        
        if (receiveResult != ErrorCode::SUCCESS) {
            Logger::getInstance().error("Failed to receive response from daemon");
            return receiveResult;
        }

        response = MessageSerializer::deserializeResponse(serializedResponse);
        if (!response) {
            Logger::getInstance().error("Failed to deserialize response from daemon");
            return ErrorCode::SERIALIZATION_ERROR;
        }

        return ErrorCode::SUCCESS;
    }

    std::string DaemonClient::formatResponse(const Response& response) {
        if (response.isSuccess()) {
            return response.getData();
        } else {
            return "Error: " + response.getData();
        }
    }
}
