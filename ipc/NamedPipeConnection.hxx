#ifndef NAMED_PIPE_CONNECTION_HXX
#define NAMED_PIPE_CONNECTION_HXX

#include <windows.h>
#include <string>
#include "../utils/ErrorCodes.hxx"

namespace NumberStore {
    class NamedPipeConnection {
    private:
        HANDLE pipeHandle;
        bool connected;
        std::string pipeName;

    public:
        NamedPipeConnection();
        explicit NamedPipeConnection(HANDLE handle);
        ~NamedPipeConnection();

        NamedPipeConnection(const NamedPipeConnection&) = delete;
        NamedPipeConnection& operator=(const NamedPipeConnection&) = delete;

        NamedPipeConnection(NamedPipeConnection&& other) noexcept;
        NamedPipeConnection& operator=(NamedPipeConnection&& other) noexcept;

        ErrorCode connect(const std::string& name);
        ErrorCode disconnect();
        
        ErrorCode write(const std::string& data);
        ErrorCode read(std::string& data);
        
        bool isConnected() const;
        HANDLE getHandle() const;
        
    private:
        void cleanup();
        ErrorCode readExact(char* buffer, DWORD bytesToRead, DWORD& bytesRead);
        ErrorCode writeExact(const char* buffer, DWORD bytesToWrite, DWORD& bytesWritten);
    };
}

#endif // NAMED_PIPE_CONNECTION_HXX
