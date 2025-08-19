#ifndef CLIENT_HANDLER_HXX
#define CLIENT_HANDLER_HXX

#include "../ipc/NamedPipeConnection.hxx"
#include "CommandProcessor.hxx"
#include "../protocol/MessageSerializer.hxx"
#include <memory>
#include <atomic>

namespace NumberStore {
    class ClientHandler {
    private:
        std::unique_ptr<NamedPipeConnection> connection;
        CommandProcessor& processor;
        std::atomic<bool> active;
        std::string clientId;

    public:
        ClientHandler(std::unique_ptr<NamedPipeConnection> conn, CommandProcessor& proc);
        ~ClientHandler();

        ClientHandler(const ClientHandler&) = delete;
        ClientHandler& operator=(const ClientHandler&) = delete;

        void run();
        void stop();
        bool isActive() const;
        const std::string& getClientId() const;
        
    private:
        bool handleSingleCommand();
        void cleanup();
        std::string generateClientId();
    };
}

#endif // CLIENT_HANDLER_HXX
