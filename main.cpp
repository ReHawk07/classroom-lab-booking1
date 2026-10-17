#include "BookingSystem.h"
#include "ConsoleUI.h"

#include <iostream>
#include <string>

int main() {
    BookingSystem system;

    std::cout << "=== Classroom & Lab Slot Booking ===\n"
                 "Days: 1=Mon..6=Sat   Periods: 1..10 (08:00-16:55 start times)\n"
                 "Purpose: 1=Lecture 2=Tutorial 3=Practical 4=Project\n";

    for (;;) {
        std::cout << "\n1 Book  2 Room day view  3 List rooms  0 Exit\n> ";
        int choice;
        if (!(std::cin >> choice) || choice == 0) {
            break;
        }

        std::string roomId;
        std::string user;
        int purpose = 0;
        int strength = 0;
        int day = 0;
        int from = 0;
        int to = 0;
        bool inputOk = true;

        switch (choice) {
            case 1:
                inputOk = readStr("Room id: ", roomId) &&
                          readStr("Booked by (no spaces): ", user) &&
                          readInt("Purpose: ", purpose) &&
                          readInt("Strength: ", strength) &&
                          readInt("Day: ", day) &&
                          readInt("From period: ", from) &&
                          readInt("To period: ", to);
                if (inputOk) {
                    system.book(roomId, user, purpose, strength, day, from, to);
                }
                break;

            case 2:
                inputOk = readStr("Room id: ", roomId) && readInt("Day: ", day);
                if (inputOk) {
                    system.showDay(roomId, day);
                }
                break;

            case 3:
                system.listRooms();
                break;

            default:
                std::cout << "  ! Invalid choice.\n";
                break;
        }

        if (!inputOk) {
            break;
        }
    }

    return 0;
}
