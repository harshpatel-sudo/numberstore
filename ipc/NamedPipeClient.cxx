#include "NamedPipeClient.hxx"
#include "../utils/Logger.hxx"

namespace NumberStore {
    NamedPipeClient::NamedPipeClient() : connection(std::make_unique<NamedPipeConnection>()) {
    }

    NamedPipeClient::~NamedPipeClient() {
        disconnect();
    }

    ErrorCode NamedPipeClient::connect(const std::string& pipeName) {
        if (connection->isConnected()) {
            return ErrorCode::SUCCESS;
        }

        ErrorCode result = connection->connect(pipeName);
        if (result == ErrorCode::SUCCESS) {
            Logger::getInstance().info("Client connected to daemon");
        } else {
            Logger::getInstance().error("Failed to connect to daemon");
        }

        return result;
    }

    ErrorCode NamedPipeClient::disconnect() {
        if (!connection->isConnected()) {
            return ErrorCode::SUCCESS;
        }

        ErrorCode result = connection->disconnect();
        if (result == ErrorCode::SUCCESS) {
            Logger::getInstance().info("Client disconnected from daemon");
        }

        return result;
    }

    ErrorCode NamedPipeClient::sendMessage(const std::string& message) {
        if (!connection->isConnected()) {
            return ErrorCode::CONNECTION_FAILED;
        }

        return connection->write(message);
    }

    ErrorCode NamedPipeClient::receiveMessage(std::string& message) {
        if (!connection->isConnected()) {
            return ErrorCode::CONNECTION_FAILED;
        }

        return connection->read(message);
    }

    bool NamedPipeClient::isConnected() const {
        return connection && connection->isConnected();
    }
}
