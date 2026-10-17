#ifndef BOOKING_SYSTEM_H
#define BOOKING_SYSTEM_H

#include "RoomTree.h"
#include <string>

class BookingSystem {
private:
    RoomTree rooms_;
    int nextId_ = 1001;

public:
    BookingSystem();

    void book(const std::string& roomId, const std::string& user,
              int purpose, int strength, int day, int from, int to);
    void showDay(const std::string& roomId, int day) const;
    void listRooms() const;
};

#endif // BOOKING_SYSTEM_H
