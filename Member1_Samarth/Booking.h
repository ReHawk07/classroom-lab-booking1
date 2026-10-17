#ifndef BOOKING_H
#define BOOKING_H

#include "Common.h"
#include <string>

// One booking is a node in a room's sorted linked list for a day.
struct Booking {
    int id = 0;
    std::string user;
    Purpose purpose = Purpose::Lecture;
    int strength = 0;
    int day = 0;
    int from = 0;
    int to = 0;
    Booking* next = nullptr;
};

#endif // BOOKING_H
