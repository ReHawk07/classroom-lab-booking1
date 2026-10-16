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
/* ---------------- Derived classes ---------------- */
class LectureTheatre : public Room {
public:
    LectureTheatre(string id, int cap) : Room(move(id), cap) {}
    string typeName() const override { return "Lecture Theatre"; }
    bool allows(Purpose p) const override { return p == Purpose::Lecture; }
};
class Classroom : public Room {
public:
    Classroom(string id, int cap) : Room(move(id), cap) {}
    string typeName() const override { return "Classroom"; }
    bool allows(Purpose p) const override { return p != Purpose::Practical; }
};
class Lab : public Room {
public:
    Lab(string id, int cap) : Room(move(id), cap) {}
    string typeName() const override { return "Lab"; }
    bool allows(Purpose p) const override { return p == Purpose::Practical || p == Purpose::Project; }
};
/* ---------------- Binary search tree of rooms ---------------- */
class RoomTree {
    struct Node {
        Room *room; Node *l = nullptr, *r = nullptr;
        explicit Node(Room *x) : room(x) {}
    };
    Node *root_ = nullptr;
    static void destroy(Node *n) { if (!n) return; destroy(n->l); destroy(n->r); delete n->room; delete n; }
    template <class Fn> static void walk(Node *n, Fn &fn) {
        if (!n) return;
        walk(n->l, fn); fn(*n->room); walk(n->r, fn);       // in-order = sorted by room id
    }
public:
    ~RoomTree() { destroy(root_); }
    Room *find(const string &id) const {
        Node *n = root_;
        while (n) {
            if (id == n->room->id()) return n->room;
            n = id < n->room->id() ? n->l : n->r;
        }
        return nullptr;
    }
    void insert(Room *room) {
        Node **pp = &root_;
        while (*pp) {
            if (room->id() == (*pp)->room->id()) { delete room; return; }   // duplicate id
            pp = room->id() < (*pp)->room->id() ? &(*pp)->l : &(*pp)->r;
        }
        *pp = new Node(room);
    }
    template <class Fn> void inorder(Fn fn) const { walk(root_, fn); }
};

/* ---------------- The system ---------------- */
class BookingSystem {
    RoomTree rooms_;
    int nextId_ = 1001;
public:
    BookingSystem() {                                       // sample rooms
        rooms_.insert(new LectureTheatre("124", 120));
        rooms_.insert(new LectureTheatre("224", 120));
        rooms_.insert(new LectureTheatre("206", 100));
        rooms_.insert(new Classroom("117", 60));
        rooms_.insert(new Classroom("114", 60));
        rooms_.insert(new Lab("009", 40));
        rooms_.insert(new Lab("209", 40));
    }

    void book(const string &roomId, const string &user, int purpose, int strength, int day, int from, int to) {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        if (day < 1 || day > DAYS || from < 1 || to > PERIODS || from > to || purpose < 1 || purpose > 4) {
            cout << "  ! Invalid day, period or purpose.\n"; return;
        }
        Purpose p = static_cast<Purpose>(purpose);
        if (!r->allows(p)) {                                // polymorphic check
            cout << "  ! " << r->typeName() << " cannot host a " << purposeName(p) << ".\n"; return;
        }
        if (strength > r->capacity()) { cout << "  ! Exceeds capacity " << r->capacity() << ".\n"; return; }
        if (Booking *c = r->conflict(day, from, to)) {
            cout << "  ! Clash with #" << c->id << " (" << c->user << ", periods " << c->from << "-"
                 << c->to << ").\n";
            return;
        }
        Booking *b = new Booking;
        b->id = nextId_++; b->user = user; b->purpose = p; b->strength = strength;
        b->day = day; b->from = from; b->to = to;
        r->insert(b);
        cout << "  Booked #" << b->id << ": " << r->id() << " " << DAY_NAME[day] << " P" << from << "-" << to
             << " (" << PERIOD_TIME[from] << "-" << PERIOD_TIME[to] << ") for " << user << "\n";
    }

    void showDay(const string &roomId, int day) const {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        if (day < 1 || day > DAYS) { cout << "  ! Invalid day.\n"; return; }
        bool used[PERIODS + 2] = {};
        cout << "  " << r->id() << " " << DAY_NAME[day] << ":\n";
        for (Booking *b = r->daySlots(day); b; b = b->next) {
            cout << "    P" << b->from << "-" << b->to << " (" << PERIOD_TIME[b->from] << ")  #" << b->id
                 << "  " << left << setw(10) << purposeName(b->purpose) << b->user << ", "
                 << b->strength << " students\n";
            for (int i = b->from; i <= b->to; i++) used[i] = true;
        }
        cout << "    Free periods:";
        for (int i = 1; i <= PERIODS; i++) if (!used[i]) cout << " " << i;
        cout << "\n";
    }

    void listRooms() const { rooms_.inorder([](Room &r) { r.describe(); }); }   // polymorphic call
};
/* ---------------- Console UI ---------------- */
static bool readInt(const string &q, int &v) { cout << q; return static_cast<bool>(cin >> v); }
static bool readStr(const string &q, string &s) { cout << q; return static_cast<bool>(cin >> s); }

int main() {
    BookingSystem sys;
    cout << "=== Classroom & Lab Slot Booking ===\nDays: 1=Mon..6=Sat   Periods: 1..10 (08:00-17:50)\n"
            "Purpose: 1=Lecture 2=Tutorial 3=Practical 4=Project\n";
    for (;;) {
        cout << "\n1 Book  2 Room day view  3 List rooms  0 Exit\n> ";
        int ch;
        if (!(cin >> ch) || ch == 0) break;
        string rid, user; int p, s, d, f, t;
        bool ok = true;
        switch (ch) {
        case 1:
            ok = readStr("Room id: ", rid) && readStr("Booked by (no spaces): ", user) &&
                 readInt("Purpose: ", p) && readInt("Strength: ", s) && readInt("Day: ", d) &&
                 readInt("From period: ", f) && readInt("To period: ", t);
            if (ok) sys.book(rid, user, p, s, d, f, t);
            break;
        case 2: if ((ok = readStr("Room id: ", rid) && readInt("Day: ", d))) sys.showDay(rid, d); break;
        case 3: sys.listRooms(); break;
        default: cout << "  ! Invalid choice.\n";
        }
        if (!ok) break;
    }
    return 0;
}
