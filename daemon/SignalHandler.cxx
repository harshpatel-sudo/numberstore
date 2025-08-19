#include "SignalHandler.hxx"
#include "DaemonServer.hxx"
#include "../utils/Logger.hxx"
#include <windows.h>
#include <signal.h>

namespace NumberStore {
    std::atomic<bool> SignalHandler::shutdownRequested{false};
    DaemonServer* SignalHandler::serverInstance = nullptr;
    bool SignalHandler::initialized = false;

    BOOL WINAPI ConsoleHandler(DWORD signal) {
        if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT) {
            Logger::getInstance().info("Shutdown signal received");
            SignalHandler::requestShutdown();
            return TRUE;
        }
        return FALSE;
    }

    void SignalHandler::setupSignalHandlers() {
        if (initialized) {
            return;
        }

        // Set up console control handler for Windows
        if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
            Logger::getInstance().error("Failed to set console control handler");
        }

        // Set up signal handlers for standard signals
        signal(SIGINT, signalCallback);
        signal(SIGTERM, signalCallback);

        initialized = true;
        Logger::getInstance().info("Signal handlers initialized");
    }

    void SignalHandler::signalCallback(int signal) {
        Logger::getInstance().info("Signal received: " + std::to_string(signal));
        requestShutdown();
    }

    bool SignalHandler::isShutdownRequested() {
        return shutdownRequested.load();
    }

    void SignalHandler::requestShutdown() {
        if (!shutdownRequested.exchange(true)) {
            Logger::getInstance().info("Shutdown requested");
            serverInstance->stop();
        }
    }

    void SignalHandler::registerServer(DaemonServer* server) {
        serverInstance = server;
        Logger::getInstance().debug("Daemon server registered with signal handler");
    }

    void SignalHandler::cleanup() {
        if (initialized) {
            SetConsoleCtrlHandler(ConsoleHandler, FALSE);
            signal(SIGINT, SIG_DFL);
            signal(SIGTERM, SIG_DFL);
            
            serverInstance = nullptr;
            shutdownRequested.store(false);
            initialized = false;
            
            Logger::getInstance().debug("Signal handler cleanup completed");
        }
    }
}
