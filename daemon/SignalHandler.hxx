#ifndef SIGNAL_HANDLER_HXX
#define SIGNAL_HANDLER_HXX

#include <atomic>
#include <functional>

namespace NumberStore {
    class DaemonServer; // Forward declaration

    class SignalHandler {
    private:
        static std::atomic<bool> shutdownRequested;
        static DaemonServer* serverInstance;
        static bool initialized;

    public:
        static void setupSignalHandlers();
        static void signalCallback(int signal);
        static bool isShutdownRequested();
        static void requestShutdown();
        static void registerServer(DaemonServer* server);
        
        // Cleanup method
        static void cleanup();
        
    private:
        static void handleConsoleEvent();
    };
}

#endif // SIGNAL_HANDLER_HXX
