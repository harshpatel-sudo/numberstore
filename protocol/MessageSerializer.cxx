#include "MessageSerializer.hxx"
#include "../utils/Logger.hxx"

namespace NumberStore {
    const std::string MESSAGE_TERMINATOR = "\n";

    std::string MessageSerializer::serialize(const Message& message) {
        return addMessageTerminator(message.serialize());
    }

    std::unique_ptr<Message> MessageSerializer::deserialize(const std::string& data) {
        if (!isValidMessageFormat(data)) {
            Logger::getInstance().error("Invalid message format");
            return nullptr;
        }

        std::string cleanData = removeMessageTerminator(data);
        return Message::deserialize(cleanData);
    }

    std::string MessageSerializer::serializeCommand(const Command& command) {
        return addMessageTerminator(command.serialize());
    }

    std::string MessageSerializer::serializeResponse(const Response& response) {
        return addMessageTerminator(response.serialize());
    }

    std::unique_ptr<Command> MessageSerializer::deserializeCommand(const std::string& data) {
        auto message = deserialize(data);
        if (!message || message->getType() != MessageType::COMMAND) {
            return nullptr;
        }

        // Safe cast since type is verified
        return std::unique_ptr<Command>(static_cast<Command*>(message.release()));
    }

    std::unique_ptr<Response> MessageSerializer::deserializeResponse(const std::string& data) {
        auto message = deserialize(data);
        if (!message || message->getType() != MessageType::RESPONSE) {
            return nullptr;
        }

        // Safe cast since type is verified
        return std::unique_ptr<Response>(static_cast<Response*>(message.release()));
    }

    bool MessageSerializer::isValidMessageFormat(const std::string& data) {
        if (data.empty()) {
            return false;
        }

        // Check for basic protocol format: TYPE:CONTENT\n
        size_t colonPos = data.find(':');
        if (colonPos == std::string::npos) {
            return false;
        }

        // Check for message terminator
        if (data.back() != '\n') {
            return false;
        }

        return true;
    }

    std::string MessageSerializer::addMessageTerminator(const std::string& message) {
        return message + MESSAGE_TERMINATOR;
    }

    std::string MessageSerializer::removeMessageTerminator(const std::string& message) {
        if (!message.empty() && message.back() == '\n') {
            return message.substr(0, message.length() - 1);
        }
        return message;
    }
}
