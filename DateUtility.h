#ifndef DATEUTILITY_H
#define DATEUTILITY_H

#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

class DateUtility {
public:
    static time_t getCurrentTime() {
        return std::time(nullptr);
    }

    static std::string timeToString(time_t time) {
        std::tm tm = *std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static time_t getPastDays(int k) {
        return getCurrentTime() - static_cast<time_t>(k) * 86400;
    }

    static time_t getPastMonth() {
        return getPastDays(30);
    }
};

#endif