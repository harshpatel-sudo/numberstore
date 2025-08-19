#include "ErrorCodes.hxx"

namespace NumberStore {
    std::string ErrorHandler::getErrorMessage(const ErrorCode& code) {
        switch (code) {
            case ErrorCode::SUCCESS:
                return "Operation completed successfully";
            case ErrorCode::INVALID_NUMBER:
                return "Invalid number format or non-positive integer";
            case ErrorCode::DUPLICATE_NUMBER:
                return "Number already exists";
            case ErrorCode::NUMBER_NOT_FOUND:
                return "Number not found";
            case ErrorCode::CONNECTION_FAILED:
                return "Failed to establish connection";
            case ErrorCode::SERIALIZATION_ERROR:
                return "Failed to serialize/deserialize message";
            case ErrorCode::INVALID_COMMAND:
                return "Invalid command format";
            case ErrorCode::PIPE_CREATE_FAILED:
                return "Failed to create named pipe";
            case ErrorCode::PIPE_CONNECT_FAILED:
                return "Failed to connect to named pipe";
            case ErrorCode::READ_FAILED:
                return "Failed to read from pipe";
            case ErrorCode::WRITE_FAILED:
                return "Failed to write to pipe";
            case ErrorCode::TIMEOUT:
                return "Operation timed out";
            case ErrorCode::SHUTDOWN_REQUESTED:
                return "Shutdown requested";
            case ErrorCode::INITIALIZATION_FAILED:
                return "Initialization failed";
            case ErrorCode::INSTANCE_ALREADY_RUNNING:
                return "Another daemon instance is already running";
            default:
                return "Unknown error";
        }
    }

    bool ErrorHandler::isError(const ErrorCode& code) {
        return code != ErrorCode::SUCCESS;
    }
}
