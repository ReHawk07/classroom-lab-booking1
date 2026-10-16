Part 1: setup, the booking record and the Room base class (lines 1–64)

What it does: this is the foundation the rest of the code builds on.

Constants: 6 days (Mon to Sat) and 10 periods per day, with the start time of each period.
Purpose: an enum of the four reasons a room can be booked: Lecture, Tutorial, Practical or Project.
Booking: one booked slot. It stores who booked it, the day, the start and end period, and the number of students. It also has a next pointer, so bookings link together into a list.
Room: an abstract class, meaning you can’t create a plain Room. It forces each real room type to say what it is (typeName) and what it allows (allows).
Sorted list per day: each room keeps one linked list of bookings for each day, sorted by start period.
insert() puts a new booking in the right position.
conflict() checks whether a new request overlaps an existing booking. Because the list is sorted, it can stop early.
cpp
/* Classroom & Lab Slot Booking System - core part (about 40% of the project)
 * OOP: abstract Room + derived classes.  DSA: room BST, per-day sorted linked list of bookings.
 * Build: g++ -std=c++17 -Wall -Wextra -o booking booking_40.cpp */
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

constexpr int DAYS = 6;
constexpr int PERIODS = 10;
static const char *DAY_NAME[DAYS + 1] = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *PERIOD_TIME[PERIODS + 1] = {"", "08:00", "08:55", "10:10", "11:05", "12:00",
                                               "12:55", "14:10", "15:05", "16:00", "16:55"};

enum class Purpose { Lecture = 1, Tutorial, Practical, Project };
static const char *purposeName(Purpose p) {
    static const char *n[] = {"", "Lecture", "Tutorial", "Practical", "Project"};
    return n[static_cast<int>(p)];
}

/* ---------------- Booking: node of a room's sorted day list ---------------- */
struct Booking {
    int id = 0;
    string user;
    Purpose purpose = Purpose::Lecture;
    int strength = 0, day = 0, from = 0, to = 0;
    Booking *next = nullptr;
};

/* ---------------- Abstract base class ---------------- */
class Room {
protected:
    string id_;
    int capacity_;
    Booking *slots_[DAYS + 1] = {};              // one sorted linked list per day
public:
    Room(string id, int cap) : id_(move(id)), capacity_(cap) {}
    virtual ~Room() {
        for (int d = 1; d <= DAYS; d++)
            for (Booking *b = slots_[d]; b;) { Booking *n = b->next; delete b; b = n; }
    }

    virtual string typeName() const = 0;         // pure virtual -> Room is abstract
    virtual bool allows(Purpose p) const = 0;
    void describe() const {
        cout << left << setw(8) << id_ << setw(17) << typeName() << "cap " << capacity_ << "\n";
    }

    const string &id() const { return id_; }
    int capacity() const { return capacity_; }
    Booking *daySlots(int d) const { return slots_[d]; }

    Booking *conflict(int day, int from, int to) const {   // list is sorted by start period
        for (Booking *b = slots_[day]; b && b->from <= to; b = b->next)
            if (from <= b->to) return b;
        return nullptr;
    }
    void insert(Booking *b) {                              // sorted insert
        Booking **pp = &slots_[b->day];
        while (*pp && (*pp)->from < b->from) pp = &(*pp)->next;
        b->next = *pp;
        *pp = b;
    }
};
