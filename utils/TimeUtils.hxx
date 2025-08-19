#ifndef TIME_UTILS_HXX
#define TIME_UTILS_HXX

#include <cstdint>
#include <string>

namespace NumberStore {
    class TimeUtils {
    public:
        static int64_t getCurrentUnixTimestamp();
        static std::string formatTimestamp(int64_t timestamp);
        static std::string getCurrentTimestampString();
    };
}

#endif // TIME_UTILS_HXX
