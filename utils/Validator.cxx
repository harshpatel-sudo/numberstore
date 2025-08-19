#include "Validator.hxx"
#include "Constants.hxx"
#include <regex>
#include <limits>

namespace NumberStore {
    bool Validator::isValidNumber(const std::string& input) {
        if (input.empty()) return false;
        
        // Check if string contains only digits (no leading zeros except for "0")
        if (input.length() > 1 && input[0] == '0') return false;
        
        return isNumericString(input);
    }

    bool Validator::isPositiveInteger(uint64_t number) {
        return number > 0 && isInValidRange(number);
    }

    bool Validator::isValidCommand(const std::string& command) {
        return command == Constants::CMD_INSERT ||
               command == Constants::CMD_DELETE ||
               command == Constants::CMD_PRINT_ALL ||
               command == Constants::CMD_DELETE_ALL ||
               command == Constants::CMD_EXIT;
    }

    ErrorCode Validator::validateInsertInput(const std::string& input, uint64_t& outNumber) {
        if (!isValidNumber(input)) {
            return ErrorCode::INVALID_NUMBER;
        }

        try {
            uint64_t num = std::stoull(input);
            
            // Check for overflow
            if (num > std::numeric_limits<uint64_t>::max() || num <= 0) {
                return ErrorCode::INVALID_NUMBER;
            }

            outNumber = static_cast<uint64_t>(num);

            if (!isPositiveInteger(outNumber)) {
                return ErrorCode::INVALID_NUMBER;
            }
            
            return ErrorCode::SUCCESS;
        }
        catch (const std::exception&) {
            return ErrorCode::INVALID_NUMBER;
        }
    }

    ErrorCode Validator::validateDeleteInput(const std::string& input, uint64_t& outNumber) {
        // Same validation as insert for consistency
        return validateInsertInput(input, outNumber);
    }

    bool Validator::isNumericString(const std::string& str) {
        if (str.empty()) return false;
        
        for (char c : str) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }

    bool Validator::isInValidRange(uint64_t number) {
        return number > 0 && number <= std::numeric_limits<uint64_t>::max();
    }
}
