#include "TimeUtils.hxx"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace NumberStore {
    int64_t TimeUtils::getCurrentUnixTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }

    std::string TimeUtils::formatTimestamp(int64_t timestamp) {
        std::ostringstream oss;
        oss << timestamp;
        return oss.str();
    }

    std::string TimeUtils::getCurrentTimestampString() {
        return formatTimestamp(getCurrentUnixTimestamp());
    }
}
