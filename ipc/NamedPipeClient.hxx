#ifndef NAMED_PIPE_CLIENT_HXX
#define NAMED_PIPE_CLIENT_HXX

#include "NamedPipeConnection.hxx"
#include "../utils/ErrorCodes.hxx"
#include <memory>

namespace NumberStore {
    class NamedPipeClient {
    private:
        std::unique_ptr<NamedPipeConnection> connection;

    public:
        NamedPipeClient();
        ~NamedPipeClient();

        NamedPipeClient(const NamedPipeClient&) = delete;
        NamedPipeClient& operator=(const NamedPipeClient&) = delete;

        ErrorCode connect(const std::string& pipeName);
        ErrorCode disconnect();
        
        ErrorCode sendMessage(const std::string& message);
        ErrorCode receiveMessage(std::string& message);
        
        bool isConnected() const;
    };
}

#endif // NAMED_PIPE_CLIENT_HXX
