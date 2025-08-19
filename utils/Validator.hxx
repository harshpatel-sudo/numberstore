#ifndef VALIDATOR_HXX
#define VALIDATOR_HXX

#include <string>
#include "ErrorCodes.hxx"

namespace NumberStore {
    class Validator {
    public:
        static bool isValidNumber(const std::string& input);
        static bool isPositiveInteger(uint64_t number);
        static bool isValidCommand(const std::string& command);
        static ErrorCode validateInsertInput(const std::string& input, uint64_t& outNumber);
        static ErrorCode validateDeleteInput(const std::string& input, uint64_t& outNumber);

    private:
        static bool isNumericString(const std::string& str);
        static bool isInValidRange(uint64_t number);
    };
}

#endif // VALIDATOR_HXX
