# Classroom & Lab Slot Booking System

This is the original single-file booking program refactored into headers and implementation files. It uses C++17.

## Member-wise responsibility map

### Member 1 — Samarth: setup, Booking record, and Room base class
- `Member1_Samarth/Common.h` — day/period constants and booking-purpose enum/helper.
- `Member1_Samarth/Booking.h` — linked-list booking node.
- `Member1_Samarth/Room.h` / `Room.cpp` — abstract Room class, sorted per-day booking lists, conflict detection, insertion and cleanup.

### Member 2 — Mahi: room types and binary search tree
- `Member2_Mahi/RoomTypes.h` / `RoomTypes.cpp` — `LectureTheatre`, `Classroom`, and `Lab` rules.
- `Member2_Mahi/RoomTree.h` / `RoomTree.cpp` — binary search tree used to find and list rooms by ID.

### Member 3 — Ayesha: booking logic
- `Member3_Ayesha/BookingSystem.h` — system interface and declarations shared with Member 4.
- `Member3_Ayesha/BookingSystem.cpp` — initial sample rooms and the `book()` validation/booking flow.

### Member 4 — Ayush: day view, room list, and input helpers
- `Member4_Ayush/BookingSystemView.cpp` — `showDay()` and `listRooms()` implementations.
- `Member4_Ayush/ConsoleUI.h` / `ConsoleUI.cpp` — shared integer/string input helpers.

### Application entry point
- `main.cpp` — menu and program loop. It is the only file containing `main()`.

`40percent.cpp` was the complete reference program. The four `classroom_*.cpp` uploads supplied the descriptions and source snippets for the four member parts; their explanatory text was not copied into the C++ source files.

## Build

Run this command from the project root folder:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
  -IMember1_Samarth -IMember2_Mahi -IMember3_Ayesha -IMember4_Ayush \
  main.cpp \
  Member1_Samarth/Room.cpp \
  Member2_Mahi/RoomTypes.cpp Member2_Mahi/RoomTree.cpp \
  Member3_Ayesha/BookingSystem.cpp \
  Member4_Ayush/BookingSystemView.cpp Member4_Ayush/ConsoleUI.cpp \
  -o booking
```

Run the program with `./booking`. On Windows with MinGW, run `booking.exe` after compiling.

## Notes

- The room rules and menu behavior are retained from the supplied source.
- Input validation now also rejects non-positive student strengths and out-of-range period endpoints.
- The user name is still read as one token, so it cannot contain spaces, matching the original prompt.
