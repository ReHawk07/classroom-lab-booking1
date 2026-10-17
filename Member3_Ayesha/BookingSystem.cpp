#include "BookingSystem.h"

#include "RoomTypes.h"
#include <iostream>
#include <utility>

BookingSystem::BookingSystem() {
    // Sample rooms, as in the original source.
    rooms_.insert(new LectureTheatre("124", 120));
    rooms_.insert(new LectureTheatre("224", 120));
    rooms_.insert(new LectureTheatre("206", 100));
    rooms_.insert(new Classroom("117", 60));
    rooms_.insert(new Classroom("114", 60));
    rooms_.insert(new Lab("009", 40));
    rooms_.insert(new Lab("209", 40));
}

void BookingSystem::book(const std::string& roomId, const std::string& user,
                         int purpose, int strength, int day, int from, int to) {
    Room* room = rooms_.find(roomId);
    if (!room) {
        std::cout << "  ! No such room.\n";
        return;
    }

    if (day < 1 || day > DAYS || from < 1 || from > PERIODS ||
        to < 1 || to > PERIODS || from > to || purpose < 1 || purpose > 4) {
        std::cout << "  ! Invalid day, period or purpose.\n";
        return;
    }

    if (strength < 1) {
        std::cout << "  ! Strength must be at least 1.\n";
        return;
    }

    const Purpose bookingPurpose = static_cast<Purpose>(purpose);
    if (!room->allows(bookingPurpose)) {
        std::cout << "  ! " << room->typeName() << " cannot host a "
                  << purposeName(bookingPurpose) << ".\n";
        return;
    }

    if (strength > room->capacity()) {
        std::cout << "  ! Exceeds capacity " << room->capacity() << ".\n";
        return;
    }

    if (Booking* clash = room->conflict(day, from, to)) {
        std::cout << "  ! Clash with #" << clash->id << " (" << clash->user
                  << ", periods " << clash->from << "-" << clash->to << ").\n";
        return;
    }

    Booking* booking = new Booking;
    booking->id = nextId_++;
    booking->user = user;
    booking->purpose = bookingPurpose;
    booking->strength = strength;
    booking->day = day;
    booking->from = from;
    booking->to = to;

    room->insert(booking);
    std::cout << "  Booked #" << booking->id << ": " << room->id() << ' '
              << DAY_NAME[day] << " P" << from << '-' << to
              << " (" << PERIOD_TIME[from] << '-' << PERIOD_TIME[to]
              << ") for " << user << '\n';
}
