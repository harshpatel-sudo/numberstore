#ifndef MESSAGE_HXX
#define MESSAGE_HXX

#include <string>
#include <memory>

namespace NumberStore {
    enum class MessageType {
        COMMAND,
        RESPONSE
    };

    class Message {
    protected:
        MessageType type;
        std::string content;

    public:
        Message(MessageType msgType, const std::string& msgContent);
        virtual ~Message() = default;

        Message(const Message&) = default;
        Message& operator=(const Message&) = default;
        Message(Message&&) = default;
        Message& operator=(Message&&) = default;

        MessageType getType() const;
        const std::string& getContent() const;
        
        virtual std::string serialize() const = 0;
        static std::unique_ptr<Message> deserialize(const std::string& data);
    };
}

#endif // MESSAGE_HXX
