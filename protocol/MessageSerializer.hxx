#ifndef MESSAGE_SERIALIZER_HXX
#define MESSAGE_SERIALIZER_HXX

#include "Message.hxx"
#include "Command.hxx"
#include "Response.hxx"
#include "../utils/ErrorCodes.hxx"
#include <memory>

namespace NumberStore {
    class MessageSerializer {
    public:
        static std::string serialize(const Message& message);
        static std::unique_ptr<Message> deserialize(const std::string& data);
        
        // Convenience methods for specific message types
        static std::string serializeCommand(const Command& command);
        static std::string serializeResponse(const Response& response);
        
        static std::unique_ptr<Command> deserializeCommand(const std::string& data);
        static std::unique_ptr<Response> deserializeResponse(const std::string& data);
        
        // Validation
        static bool isValidMessageFormat(const std::string& data);
        
    private:
        static std::string addMessageTerminator(const std::string& message);
        static std::string removeMessageTerminator(const std::string& message);
    };
}

#endif // MESSAGE_SERIALIZER_HXX
