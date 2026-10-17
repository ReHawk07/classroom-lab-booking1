#ifndef COMMON_H
#define COMMON_H

#include <string>

constexpr int DAYS = 6;
constexpr int PERIODS = 10;

inline constexpr const char* DAY_NAME[DAYS + 1] = {
    "", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

inline constexpr const char* PERIOD_TIME[PERIODS + 1] = {
    "", "08:00", "08:55", "10:10", "11:05", "12:00",
    "12:55", "14:10", "15:05", "16:00", "16:55"
};

enum class Purpose { Lecture = 1, Tutorial, Practical, Project };

inline const char* purposeName(Purpose purpose) {
    switch (purpose) {
        case Purpose::Lecture:   return "Lecture";
        case Purpose::Tutorial:  return "Tutorial";
        case Purpose::Practical: return "Practical";
        case Purpose::Project:   return "Project";
        default:                 return "Unknown";
    }
}

#endif // COMMON_H
