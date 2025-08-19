#ifndef ERROR_CODES_HXX
#define ERROR_CODES_HXX

#include <string>

namespace NumberStore {
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_NUMBER,
        DUPLICATE_NUMBER,
        NUMBER_NOT_FOUND,
        CONNECTION_FAILED,
        SERIALIZATION_ERROR,
        INVALID_COMMAND,
        PIPE_CREATE_FAILED,
        PIPE_CONNECT_FAILED,
        READ_FAILED,
        WRITE_FAILED,
        TIMEOUT,
        SHUTDOWN_REQUESTED,
        INITIALIZATION_FAILED,
        INSTANCE_ALREADY_RUNNING
    };

    class ErrorHandler {
    public:
        static std::string getErrorMessage(const ErrorCode& code);
        static bool isError(const ErrorCode& code);
    };
}

#endif // ERROR_CODES_HXX
