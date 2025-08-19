#ifndef CONFIG_HXX
#define CONFIG_HXX

#include <string>
#include <memory>

namespace NumberStore {
    class Config {
    private:
        static std::unique_ptr<Config> instance;
        
        std::string pipeName;
        size_t connectionTimeout;
        size_t maxConnections;
        size_t bufferSize;

        Config(); // Private constructor for singleton

    public:
        ~Config() = default;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;
        
        static Config& getInstance();
        
        const std::string& getPipeName() const;
        int getConnectionTimeout() const;
        size_t getMaxConnections() const;
        size_t getBufferSize() const;
        
        void setPipeName(const std::string& name);
        void setConnectionTimeout(const size_t& timeout);
        void setMaxConnections(const size_t& max);
        void setBufferSize(const size_t& size);
        
        void loadDefaults();
    };
}

#endif // CONFIG_HXX
