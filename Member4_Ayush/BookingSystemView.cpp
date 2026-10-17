#include "BookingSystem.h"

#include <iomanip>
#include <iostream>

void BookingSystem::showDay(const std::string& roomId, int day) const {
    Room* room = rooms_.find(roomId);
    if (!room) {
        std::cout << "  ! No such room.\n";
        return;
    }
    if (day < 1 || day > DAYS) {
        std::cout << "  ! Invalid day.\n";
        return;
    }

    bool used[PERIODS + 1] = {};
    std::cout << "  " << room->id() << ' ' << DAY_NAME[day] << ":\n";
    for (Booking* booking = room->daySlots(day); booking; booking = booking->next) {
        std::cout << "    P" << booking->from << '-' << booking->to
                  << " (" << PERIOD_TIME[booking->from] << ")  #" << booking->id
                  << "  " << std::left << std::setw(10) << purposeName(booking->purpose)
                  << booking->user << ", " << booking->strength << " students\n";
        for (int period = booking->from; period <= booking->to; ++period) {
            used[period] = true;
        }
    }

    std::cout << "    Free periods:";
    for (int period = 1; period <= PERIODS; ++period) {
        if (!used[period]) {
            std::cout << ' ' << period;
        }
    }
    std::cout << '\n';
}

void BookingSystem::listRooms() const {
    rooms_.inorder([](Room& room) { room.describe(); });
}
