#ifndef ROOM_H
#define ROOM_H

#include "Booking.h"
#include <string>

class Room {
protected:
    std::string id_;
    int capacity_;
    Booking* slots_[DAYS + 1] = {}; // One sorted linked list per day.

public:
    Room(std::string id, int capacity);
    virtual ~Room();

    Room(const Room&) = delete;
    Room& operator=(const Room&) = delete;

    virtual std::string typeName() const = 0;
    virtual bool allows(Purpose purpose) const = 0;

    void describe() const;
    const std::string& id() const;
    int capacity() const;
    Booking* daySlots(int day) const;

    Booking* conflict(int day, int from, int to) const;
    void insert(Booking* booking);
};

#endif // ROOM_H
