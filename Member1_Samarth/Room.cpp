#include "Room.h"

#include <iomanip>
#include <iostream>
#include <utility>

Room::Room(std::string id, int capacity)
    : id_(std::move(id)), capacity_(capacity) {}

Room::~Room() {
    for (int day = 1; day <= DAYS; ++day) {
        Booking* booking = slots_[day];
        while (booking) {
            Booking* next = booking->next;
            delete booking;
            booking = next;
        }
    }
}

void Room::describe() const {
    std::cout << std::left << std::setw(8) << id_
              << std::setw(17) << typeName()
              << "cap " << capacity_ << '\n';
}

const std::string& Room::id() const {
    return id_;
}

int Room::capacity() const {
    return capacity_;
}

Booking* Room::daySlots(int day) const {
    return slots_[day];
}

Booking* Room::conflict(int day, int from, int to) const {
    // The list is sorted by start period, so searching can stop early.
    for (Booking* booking = slots_[day];
         booking && booking->from <= to;
         booking = booking->next) {
        if (from <= booking->to) {
            return booking;
        }
    }
    return nullptr;
}

void Room::insert(Booking* booking) {
    Booking** position = &slots_[booking->day];
    while (*position && (*position)->from < booking->from) {
        position = &(*position)->next;
    }
    booking->next = *position;
    *position = booking;
}
