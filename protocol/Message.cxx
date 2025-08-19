#include "Message.hxx"
#include "Command.hxx"
#include "Response.hxx"
#include "../utils/Logger.hxx"
#include <sstream>

namespace NumberStore {
    Message::Message(MessageType msgType, const std::string& msgContent)
        : type(msgType), content(msgContent) {
    }

    MessageType Message::getType() const {
        return type;
    }

    const std::string& Message::getContent() const {
        return content;
    }

    std::unique_ptr<Message> Message::deserialize(const std::string& data) {
        if (data.empty()) {
            Logger::getInstance().error("Empty message data for deserialization");
            return nullptr;
        }

        // Simple protocol: TYPE:CONTENT
        size_t colonPos = data.find(':');
        if (colonPos == std::string::npos) {
            Logger::getInstance().error("Invalid message format: " + data);
            return nullptr;
        }

        std::string typeStr = data.substr(0, colonPos);
        std::string content = data.substr(colonPos + 1);

        if (typeStr == "CMD") {
            return Command::deserialize(content);
        } else if (typeStr == "RESP") {
            return Response::deserialize(content);
        }

        Logger::getInstance().error("Unknown message type: " + typeStr);
        return nullptr;
    }
}
