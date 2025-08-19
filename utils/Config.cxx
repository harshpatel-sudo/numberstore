#include "Config.hxx"
#include "Constants.hxx"

namespace NumberStore {
    std::unique_ptr<Config> Config::instance = nullptr;

    Config::Config() {
        loadDefaults();
    }

    Config& Config::getInstance() {
        if (!instance) {
            instance = std::unique_ptr<Config>(new Config());
        }
        return *instance;
    }

    const std::string& Config::getPipeName() const {
        return pipeName;
    }

    size_t Config::getConnectionTimeout() const {
        return connectionTimeout;
    }

    size_t Config::getMaxConnections() const {
        return maxConnections;
    }

    size_t Config::getBufferSize() const {
        return bufferSize;
    }

    void Config::setPipeName(const std::string& name) {
        pipeName = name;
    }

    void Config::setConnectionTimeout(const size_t& timeout) {
        connectionTimeout = timeout;
    }

    void Config::setMaxConnections(const size_t& max) {
        maxConnections = max;
    }

    void Config::setBufferSize(const size_t& size) {
        bufferSize = size;
    }

    void Config::loadDefaults() {
        pipeName = Constants::PIPE_NAME;
        connectionTimeout = Constants::DEFAULT_TIMEOUT;
        maxConnections = Constants::MAX_CONNECTIONS;
        bufferSize = Constants::BUFFER_SIZE;
    }
}
