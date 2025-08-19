#ifndef CONSTANTS_HXX
#define CONSTANTS_HXX

#include <string>

namespace NumberStore {
    namespace Constants {
        // Named Pipe Configuration
        const std::string PIPE_NAME = "\\\\.\\pipe\\numberstore";
        const int DEFAULT_TIMEOUT = 5000; // milliseconds
        const size_t MAX_CONNECTIONS = 100;
        const size_t BUFFER_SIZE = 4096;
        
        // Protocol Messages
        const std::string CMD_INSERT = "INSERT";
        const std::string CMD_DELETE = "DELETE";
        const std::string CMD_PRINT_ALL = "PRINT_ALL";
        const std::string CMD_DELETE_ALL = "DELETE_ALL";
        const std::string CMD_EXIT = "EXIT";
        
        const std::string RESP_SUCCESS = "SUCCESS";
        const std::string RESP_ERROR = "ERROR";
        const std::string RESP_DATA = "DATA";
        
        // Messages
        const std::string MSG_ENTER_NUMBER = "Enter a positive integer: ";
        const std::string MSG_INVALID_INPUT = "Invalid input. Please enter a positive integer.";
        const std::string MSG_GOODBYE = "Goodbye!";
    }
}

#endif // CONSTANTS_HXX
