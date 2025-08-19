#include "Command.hxx"
#include "../utils/Constants.hxx"
#include "../utils/Logger.hxx"
#include <sstream>

namespace NumberStore {
    Command::Command(CommandType cmdType, uint64_t num)
        : Message(MessageType::COMMAND, ""), commandType(cmdType), number(num) {
    }

    CommandType Command::getCommandType() const {
        return commandType;
    }

    uint64_t Command::getNumber() const {
        return number;
    }

    std::string Command::serialize() const {
        std::ostringstream oss;
        oss << "CMD:" << commandTypeToString(commandType);
        
        if (commandType == CommandType::INSERT || commandType == CommandType::DELETE_NUM) {
            oss << " " << number;
        }
        
        return oss.str();
    }

    std::unique_ptr<Command> Command::deserialize(const std::string& content) {
        std::istringstream iss(content);
        std::string commandStr;
        iss >> commandStr;

        CommandType cmdType = stringToCommandType(commandStr);
        
        if (cmdType == CommandType::INSERT || cmdType == CommandType::DELETE_NUM) {
            uint64_t num;
            if (!(iss >> num)) {
                Logger::getInstance().error("Missing number for command: " + commandStr);
                return nullptr;
            }
            return std::make_unique<Command>(cmdType, num);
        } else {
            return std::make_unique<Command>(cmdType);
        }
    }

    std::unique_ptr<Command> Command::createInsertCommand(uint64_t number) {
        return std::make_unique<Command>(CommandType::INSERT, number);
    }

    std::unique_ptr<Command> Command::createDeleteCommand(uint64_t number) {
        return std::make_unique<Command>(CommandType::DELETE_NUM, number);
    }

    std::unique_ptr<Command> Command::createPrintAllCommand() {
        return std::make_unique<Command>(CommandType::PRINT_ALL);
    }

    std::unique_ptr<Command> Command::createDeleteAllCommand() {
        return std::make_unique<Command>(CommandType::DELETE_ALL);
    }

    std::unique_ptr<Command> Command::createExitCommand() {
        return std::make_unique<Command>(CommandType::EXIT);
    }

    CommandType Command::stringToCommandType(const std::string& str) {
        if (str == Constants::CMD_INSERT) return CommandType::INSERT;
        if (str == Constants::CMD_DELETE) return CommandType::DELETE_NUM;
        if (str == Constants::CMD_PRINT_ALL) return CommandType::PRINT_ALL;
        if (str == Constants::CMD_DELETE_ALL) return CommandType::DELETE_ALL;
        if (str == Constants::CMD_EXIT) return CommandType::EXIT;
        
        Logger::getInstance().error("Unknown command type: " + str);
        return CommandType::EXIT;
    }

    std::string Command::commandTypeToString(CommandType type) {
        switch (type) {
            case CommandType::INSERT: return Constants::CMD_INSERT;
            case CommandType::DELETE_NUM: return Constants::CMD_DELETE;
            case CommandType::PRINT_ALL: return Constants::CMD_PRINT_ALL;
            case CommandType::DELETE_ALL: return Constants::CMD_DELETE_ALL;
            case CommandType::EXIT: return Constants::CMD_EXIT;
            default: return Constants::CMD_EXIT;
        }
    }
}
