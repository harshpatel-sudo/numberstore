#ifndef NAMED_PIPE_SERVER_HXX
#define NAMED_PIPE_SERVER_HXX

#include <windows.h>
#include <string>
#include <memory>
#include "NamedPipeConnection.hxx"
#include "../utils/ErrorCodes.hxx"

namespace NumberStore {
    class NamedPipeServer {
    private:
        std::string pipeName;
        bool listening;

    public:
        NamedPipeServer();
        ~NamedPipeServer();

        NamedPipeServer(const NamedPipeServer&) = delete;
        NamedPipeServer& operator=(const NamedPipeServer&) = delete;

        ErrorCode start(const std::string& name);
        ErrorCode stop();
        
        std::unique_ptr<NamedPipeConnection> acceptConnection();
        
        bool isListening() const;
        const std::string& getPipeName() const;
        
    private:
        HANDLE createNamedPipeInstance();
    };
}

#endif // NAMED_PIPE_SERVER_HXX
