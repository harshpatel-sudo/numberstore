#include "NamedPipeServer.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"

namespace NumberStore {
    NamedPipeServer::NamedPipeServer() : listening(false) {
    }

    NamedPipeServer::~NamedPipeServer() {
        stop();
    }

    ErrorCode NamedPipeServer::start(const std::string& name) {
        if (listening) {
            return ErrorCode::SUCCESS;
        }

        pipeName = name;
        listening = true;
        
        Logger::getInstance().info("Named pipe server started: " + pipeName);
        return ErrorCode::SUCCESS;
    }

    ErrorCode NamedPipeServer::stop() {
        if (!listening) {
            return ErrorCode::SUCCESS;
        }

        listening = false;
        Logger::getInstance().info("Named pipe server stopped: " + pipeName);
        return ErrorCode::SUCCESS;
    }

    std::unique_ptr<NamedPipeConnection> NamedPipeServer::acceptConnection() {
        if (!listening) {
            return nullptr;
        }

        HANDLE pipeHandle = createNamedPipeInstance();
        if (pipeHandle == INVALID_HANDLE_VALUE) {
            return nullptr;
        }

        // Wait for client to connect
        if (ConnectNamedPipe(pipeHandle, nullptr)) {
            Logger::getInstance().info("Client connected to pipe");
            return std::make_unique<NamedPipeConnection>(pipeHandle);
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_PIPE_CONNECTED) {
                // Client already connected
                Logger::getInstance().info("Client already connected to pipe");
                return std::make_unique<NamedPipeConnection>(pipeHandle);
            } else {
                Logger::getInstance().error("ConnectNamedPipe failed: " + std::to_string(error));
                CloseHandle(pipeHandle);
                return nullptr;
            }
        }
    }

    bool NamedPipeServer::isListening() const {
        return listening;
    }

    const std::string& NamedPipeServer::getPipeName() const {
        return pipeName;
    }

    HANDLE NamedPipeServer::createNamedPipeInstance() {
        Config& config = Config::getInstance();
        
        HANDLE pipeHandle = CreateNamedPipeA(
            pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            static_cast<DWORD>(config.getBufferSize()),
            static_cast<DWORD>(config.getBufferSize()),
            0,
            nullptr
        );

        if (pipeHandle == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            Logger::getInstance().error("CreateNamedPipe failed: " + std::to_string(error));
        }

        return pipeHandle;
    }
}
