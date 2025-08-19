#ifndef DAEMON_CLIENT_HXX
#define DAEMON_CLIENT_HXX

#include "../ipc/NamedPipeClient.hxx"
#include "../protocol/Command.hxx"
#include "../protocol/Response.hxx"
#include "../protocol/MessageSerializer.hxx"
#include "../utils/ErrorCodes.hxx"
#include <memory>

namespace NumberStore {
    class DaemonClient {
    private:
        std::unique_ptr<NamedPipeClient> client;
        bool connected;

    public:
        DaemonClient();
        ~DaemonClient();

        DaemonClient(const DaemonClient&) = delete;
        DaemonClient& operator=(const DaemonClient&) = delete;

        ErrorCode connect();
        ErrorCode disconnect();
        
        ErrorCode sendCommand(const Command& command, std::unique_ptr<Response>& response);
        
        // Convenience methods for specific commands
        ErrorCode insertNumber(uint64_t number, std::string& result);
        ErrorCode deleteNumber(uint64_t number, std::string& result);
        ErrorCode printAllNumbers(std::string& result);
        ErrorCode deleteAllNumbers(std::string& result);
        ErrorCode exitSession(std::string& result);
        
        bool isConnected() const;
        
    private:
        ErrorCode sendCommandInternal(const Command& command, std::unique_ptr<Response>& response);
        std::string formatResponse(const Response& response);
    };
}

#endif // DAEMON_CLIENT_HXX
