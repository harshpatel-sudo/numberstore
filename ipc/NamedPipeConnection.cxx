#include "NamedPipeConnection.hxx"
#include "../utils/Logger.hxx"
#include "../utils/Config.hxx"
#include <iostream>

namespace NumberStore {
    NamedPipeConnection::NamedPipeConnection() 
        : pipeHandle(INVALID_HANDLE_VALUE), connected(false) {
    }

    NamedPipeConnection::NamedPipeConnection(HANDLE handle) 
        : pipeHandle(handle), connected(handle != INVALID_HANDLE_VALUE) {
    }

    NamedPipeConnection::~NamedPipeConnection() {
        cleanup();
    }

    NamedPipeConnection::NamedPipeConnection(NamedPipeConnection&& other) noexcept
        : pipeHandle(other.pipeHandle), connected(other.connected), pipeName(std::move(other.pipeName)) {
        other.pipeHandle = INVALID_HANDLE_VALUE;
        other.connected = false;
    }

    NamedPipeConnection& NamedPipeConnection::operator=(NamedPipeConnection&& other) noexcept {
        if (this != &other) {
            cleanup();
            pipeHandle = other.pipeHandle;
            connected = other.connected;
            pipeName = std::move(other.pipeName);
            
            other.pipeHandle = INVALID_HANDLE_VALUE;
            other.connected = false;
        }
        return *this;
    }

    ErrorCode NamedPipeConnection::connect(const std::string& name) {
        if (connected) {
            return ErrorCode::SUCCESS;
        }

        pipeName = name;
        Config& config = Config::getInstance();

        // Wait for named pipe to be available
        if (!WaitNamedPipeA(name.c_str(), config.getConnectionTimeout())) {
            DWORD error = GetLastError();
            Logger::getInstance().error("WaitNamedPipe failed: " + std::to_string(error));
            return ErrorCode::PIPE_CONNECT_FAILED;
        }

        // Open the named pipe
        pipeHandle = CreateFileA(
            name.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (pipeHandle == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            Logger::getInstance().error("CreateFile failed: " + std::to_string(error));
            return ErrorCode::PIPE_CONNECT_FAILED;
        }

        // Set pipe mode to message mode
        DWORD mode = PIPE_READMODE_MESSAGE;
        if (!SetNamedPipeHandleState(pipeHandle, &mode, nullptr, nullptr)) {
            DWORD error = GetLastError();
            Logger::getInstance().warning("SetNamedPipeHandleState failed: " + std::to_string(error));
            // Continue anyway, as this is not critical
        }

        connected = true;
        Logger::getInstance().info("Connected to pipe: " + name);
        return ErrorCode::SUCCESS;
    }

    ErrorCode NamedPipeConnection::disconnect() {
        if (!connected) {
            return ErrorCode::SUCCESS;
        }

        cleanup();
        Logger::getInstance().info("Disconnected from pipe: " + pipeName);
        return ErrorCode::SUCCESS;
    }

    ErrorCode NamedPipeConnection::write(const std::string& data) {
        if (!connected || pipeHandle == INVALID_HANDLE_VALUE) {
            return ErrorCode::CONNECTION_FAILED;
        }

        DWORD bytesWritten;
        ErrorCode result = writeExact(data.c_str(), static_cast<DWORD>(data.length()), bytesWritten);
        
        if (result == ErrorCode::SUCCESS) {
            Logger::getInstance().debug("Sent: " + data.substr(0, data.length() - 1)); // Remove newline for logging
        }
        
        return result;
    }

    ErrorCode NamedPipeConnection::read(std::string& data) {
        if (!connected || pipeHandle == INVALID_HANDLE_VALUE) {
            return ErrorCode::CONNECTION_FAILED;
        }

        data.clear();
        char buffer[4096];
        
        // Read until we find the message terminator (\n)
        while (true) {
            DWORD bytesRead;
            
            if (!ReadFile(pipeHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
                DWORD error = GetLastError();
                if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
                    connected = false;
                    return ErrorCode::CONNECTION_FAILED;
                }
                Logger::getInstance().error("ReadFile failed: " + std::to_string(error));
                return ErrorCode::READ_FAILED;
            }

            if (bytesRead == 0) {
                connected = false;
                return ErrorCode::CONNECTION_FAILED;
            }

            buffer[bytesRead] = '\0';
            data += std::string(buffer, bytesRead);
            
            // Check if we have received the complete message (ends with \n)
            if (!data.empty() && data.back() == '\n') {
                break;
            }
        }
        
        Logger::getInstance().debug("Received: " + data.substr(0, data.length() - 1)); // Remove newline for logging
        return ErrorCode::SUCCESS;
    }

    bool NamedPipeConnection::isConnected() const {
        return connected && pipeHandle != INVALID_HANDLE_VALUE;
    }

    HANDLE NamedPipeConnection::getHandle() const {
        return pipeHandle;
    }

    void NamedPipeConnection::cleanup() {
        if (pipeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(pipeHandle);
            pipeHandle = INVALID_HANDLE_VALUE;
        }
        connected = false;
    }

    ErrorCode NamedPipeConnection::readExact(char* buffer, DWORD bytesToRead, DWORD& bytesRead) {
        bytesRead = 0;
        
        while (bytesRead < bytesToRead) {
            DWORD currentBytesRead;
            if (!ReadFile(pipeHandle, buffer + bytesRead, bytesToRead - bytesRead, &currentBytesRead, nullptr)) {
                DWORD error = GetLastError();
                Logger::getInstance().error("ReadFile failed: " + std::to_string(error));
                return ErrorCode::READ_FAILED;
            }
            
            if (currentBytesRead == 0) {
                // Connection closed
                return ErrorCode::CONNECTION_FAILED;
            }
            
            bytesRead += currentBytesRead;
        }
        
        return ErrorCode::SUCCESS;
    }

    ErrorCode NamedPipeConnection::writeExact(const char* buffer, DWORD bytesToWrite, DWORD& bytesWritten) {
        bytesWritten = 0;
        
        while (bytesWritten < bytesToWrite) {
            DWORD currentBytesWritten;
            if (!WriteFile(pipeHandle, buffer + bytesWritten, bytesToWrite - bytesWritten, &currentBytesWritten, nullptr)) {
                DWORD error = GetLastError();
                Logger::getInstance().error("WriteFile failed: " + std::to_string(error));
                return ErrorCode::WRITE_FAILED;
            }
            
            bytesWritten += currentBytesWritten;
        }
        
        return ErrorCode::SUCCESS;
    }
}
