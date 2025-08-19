#ifndef COMMAND_PROCESSOR_HXX
#define COMMAND_PROCESSOR_HXX

#include "../protocol/Command.hxx"
#include "../protocol/Response.hxx"
#include "../storage/NumberStore.hxx"
#include "../utils/ErrorCodes.hxx"
#include <memory>

namespace NumberStore {
    class CommandProcessor {
    private:
        NumberStore& numberStore;

    public:
        explicit CommandProcessor(NumberStore& store);
        ~CommandProcessor() = default;

        CommandProcessor(const CommandProcessor&) = delete;
        CommandProcessor& operator=(const CommandProcessor&) = delete;

        std::unique_ptr<Response> processCommand(const Command& command);
        
    private:
        std::unique_ptr<Response> processInsert(uint64_t number);
        std::unique_ptr<Response> processDelete(uint64_t number);
        std::unique_ptr<Response> processPrintAll();
        std::unique_ptr<Response> processDeleteAll();
        std::unique_ptr<Response> processExit();

        std::string createSuccessMessage(const std::string& operation, uint64_t number = 0, int64_t timestamp = 0);
    };
}

#endif // COMMAND_PROCESSOR_HXX
