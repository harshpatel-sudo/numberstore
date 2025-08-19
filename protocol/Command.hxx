#ifndef COMMAND_HXX
#define COMMAND_HXX

#include "Message.hxx"
#include "../utils/ErrorCodes.hxx"

namespace NumberStore {
    enum class CommandType {
        INSERT,
        DELETE_NUM,
        PRINT_ALL,
        DELETE_ALL,
        EXIT
    };

    class Command : public Message {
    private:
        CommandType commandType;
        uint64_t number; // Used for INSERT and DELETE commands

    public:
        Command(CommandType cmdType, uint64_t num = 0);
        
        CommandType getCommandType() const;
        uint64_t getNumber() const;
        
        std::string serialize() const override;
        static std::unique_ptr<Command> deserialize(const std::string& content);
        
        static std::unique_ptr<Command> createInsertCommand(uint64_t number);
        static std::unique_ptr<Command> createDeleteCommand(uint64_t number);
        static std::unique_ptr<Command> createPrintAllCommand();
        static std::unique_ptr<Command> createDeleteAllCommand();
        static std::unique_ptr<Command> createExitCommand();
        
    private:
        static CommandType stringToCommandType(const std::string& str);
        static std::string commandTypeToString(CommandType type);
    };
}

#endif // COMMAND_HXX
