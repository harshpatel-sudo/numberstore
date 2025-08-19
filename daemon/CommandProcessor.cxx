#include "CommandProcessor.hxx"
#include "../utils/Logger.hxx"
#include "../utils/TimeUtils.hxx"

namespace NumberStore {
    CommandProcessor::CommandProcessor(NumberStore& store) : numberStore(store) {
    }

    std::unique_ptr<Response> CommandProcessor::processCommand(const Command& command) {
        Logger::getInstance().debug("Processing command: " + std::to_string(static_cast<int>(command.getCommandType())));
        
        switch (command.getCommandType()) {
            case CommandType::INSERT:
                return processInsert(command.getNumber());
                
            case CommandType::DELETE_NUM:
                return processDelete(command.getNumber());
                
            case CommandType::PRINT_ALL:
                return processPrintAll();
                
            case CommandType::DELETE_ALL:
                return processDeleteAll();
                
            case CommandType::EXIT:
                return processExit();
                
            default:
                Logger::getInstance().error("Unknown command type");
                return Response::createErrorResponse(ErrorCode::INVALID_COMMAND);
        }
    }

    std::unique_ptr<Response> CommandProcessor::processInsert(uint64_t number) {
        ErrorCode result = numberStore.insert(number);
        
        if (result == ErrorCode::SUCCESS) {
            int64_t timestamp = TimeUtils::getCurrentUnixTimestamp();
            std::string message = createSuccessMessage("inserted", number, timestamp);
            return Response::createSuccessResponse(message);
        } else {
            return Response::createErrorResponse(result);
        }
    }

    std::unique_ptr<Response> CommandProcessor::processDelete(uint64_t number) {
        int64_t timestamp;
        ErrorCode result = numberStore.remove(number, timestamp);
        
        if (result == ErrorCode::SUCCESS) {
            std::string message = createSuccessMessage("deleted", number, timestamp);
            return Response::createSuccessResponse(message);
        } else {
            return Response::createErrorResponse(result);
        }
    }

    std::unique_ptr<Response> CommandProcessor::processPrintAll() {
        std::string data = numberStore.printAll();  
        return Response::createDataResponse(data);
    }

    std::unique_ptr<Response> CommandProcessor::processDeleteAll() {
        size_t count = numberStore.size();
        ErrorCode result = numberStore.clear();
        
        if (result == ErrorCode::SUCCESS) {
            std::string message = "Deleted all numbers (" + std::to_string(count) + " entries cleared)";
            return Response::createSuccessResponse(message);
        } else {
            return Response::createErrorResponse(result);
        }
    }

    std::unique_ptr<Response> CommandProcessor::processExit() {
        Logger::getInstance().info("Client requested exit");
        return Response::createSuccessResponse("Goodbye!");
    }

    std::string CommandProcessor::createSuccessMessage(const std::string& operation, uint64_t number, int64_t timestamp) {
        std::string message = "Number " + std::to_string(number) + " " + operation;
        
        if (timestamp > 0) {
            message += " at timestamp " + std::to_string(timestamp);
        }
        
        return message;
    }
}
