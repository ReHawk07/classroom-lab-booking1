/*
 * Classroom & Lab Slot Booking System  (C++17, OOP + Data Structures)
 *
 * OOP concepts
 *   Abstraction    : Room is an abstract class (pure virtual typeName / allows / describe)
 *   Inheritance    : LectureTheatre, Classroom, Lab, Auditorium derive from Room
 *   Polymorphism   : the system only holds Room*, virtual calls pick the right behaviour
 *   Encapsulation  : RoomTree, BookingList, WaitQueue, BookingSystem hide their internals
 *   Factory        : makeRoom(kind, id) creates the right derived class
 *
 * Data structures (no hash maps / STL containers used for the core logic)
 *   Binary search tree : RoomTree, rooms keyed by id (in-order = sorted listing)
 *   Linked list        : BookingList, global registry of user bookings (ascending id)
 *   Sorted linked list : Room::slots_, bookings of a room per day ordered by start period
 *   Queue (linked)     : WaitQueue, FIFO waiting list per room, auto-promoted on cancel
 *
 * File handling (fstream)
 *   timetable.csv : fixed weekly timetable (exported from the Excel Master List)
 *   bookings.dat  : user bookings + waiting requests, loaded at start-up, saved on exit
 *
 * Build: g++ -std=c++17 -Wall -Wextra -o booking booking.cpp      Run: ./booking
 */
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
 
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
static bool validPurpose(int p) { return p >= 1 && p <= 4; }
 
class Room;
 
/* ---------------- Booking (node of a room's sorted day list and of the global list) ---------------- */
struct Booking {
    int id = 0;                       // 0 for timetable (fixed) slots
    string user;                      // person, or subject code for timetable slots
    Purpose purpose = Purpose::Lecture;
    int strength = 0, day = 0, from = 0, to = 0;
    bool fixed = false;
    Room *room = nullptr;
    Booking *next = nullptr;          // sorted per-day list
    Booking *gnext = nullptr;         // global list (user bookings only)
};
 
/* ---------------- Global registry: singly linked list ---------------- */
class BookingList {
public:
    Booking *head = nullptr, *tail = nullptr;
    void append(Booking *b) {
        if (tail) tail->gnext = b; else head = b;
        tail = b;
    }
    Booking *find(int id) const {
        for (Booking *b = head; b; b = b->gnext) if (b->id == id) return b;
        return nullptr;
    }
    void unlink(Booking *b) {
        Booking *prev = nullptr, *cur = head;
        while (cur && cur != b) { prev = cur; cur = cur->gnext; }
        if (!cur) return;
        if (prev) prev->gnext = cur->gnext; else head = cur->gnext;
        if (tail == cur) tail = prev;
    }
};
 
/* ---------------- Waiting queue: linked FIFO ---------------- */
struct WaitRequest {
    string user;
    Purpose purpose;
    int strength, day, from, to;
    WaitRequest *next = nullptr;
};
 
class WaitQueue {
    WaitRequest *head_ = nullptr, *tail_ = nullptr;
public:
    WaitQueue() = default;
    WaitQueue(const WaitQueue &) = delete;
    WaitQueue &operator=(const WaitQueue &) = delete;
    ~WaitQueue() { while (head_) { WaitRequest *n = head_->next; delete head_; head_ = n; } }
 
    void push(const WaitRequest &r) {
        WaitRequest *n = new WaitRequest(r);
        n->next = nullptr;
        if (tail_) tail_->next = n; else head_ = n;
        tail_ = n;
    }
    template <class Pred>             // removes and returns the EARLIEST request satisfying pred
    WaitRequest *extractFirst(Pred pred) {
        WaitRequest *prev = nullptr;
        for (WaitRequest *w = head_; w; prev = w, w = w->next) {
            if (!pred(*w)) continue;
            if (prev) prev->next = w->next; else head_ = w->next;
            if (tail_ == w) tail_ = prev;
            w->next = nullptr;
            return w;
        }
        return nullptr;
    }
    template <class Fn> void forEach(Fn fn) const { for (WaitRequest *w = head_; w; w = w->next) fn(*w); }
};
 
/* ---------------- Abstract base class ---------------- */
class Room {
protected:
    string id_;
    int capacity_;
    Booking *slots_[DAYS + 1] = {};
    WaitQueue waiting_;
public:
    Room(string id, int cap) : id_(move(id)), capacity_(cap) {}
    virtual ~Room() {
        for (int d = 1; d <= DAYS; d++)
            for (Booking *b = slots_[d]; b;) { Booking *n = b->next; delete b; b = n; }
    }
    Room(const Room &) = delete;
    Room &operator=(const Room &) = delete;
 
    virtual string typeName() const = 0;
    virtual bool allows(Purpose p) const = 0;
    virtual void describe() const = 0;
 
    const string &id() const { return id_; }
    int capacity() const { return capacity_; }
    Booking *daySlots(int d) const { return slots_[d]; }
    WaitQueue &waiting() { return waiting_; }
    const WaitQueue &waiting() const { return waiting_; }
 
    Booking *conflict(int day, int from, int to) const {          // list is sorted by start period
        for (Booking *b = slots_[day]; b && b->from <= to; b = b->next)
            if (b->from <= to && from <= b->to) return b;
        return nullptr;
    }
    void insert(Booking *b) {                                     // sorted insert
        b->room = this;
        Booking **pp = &slots_[b->day];
        while (*pp && (*pp)->from < b->from) pp = &(*pp)->next;
        b->next = *pp;
        *pp = b;
    }
    void remove(Booking *b) {
        Booking **pp = &slots_[b->day];
        while (*pp && *pp != b) pp = &(*pp)->next;
        if (*pp) *pp = b->next;
    }
};
 
/* ---------------- Derived classes ---------------- */
class LectureTheatre : public Room {
    bool projector_;
public:
    LectureTheatre(string id, int cap, bool proj) : Room(move(id), cap), projector_(proj) {}
    string typeName() const override { return "Lecture Theatre"; }
    bool allows(Purpose p) const override { return p == Purpose::Lecture; }
    void describe() const override {
        cout << left << setw(22) << id_ << setw(17) << typeName() << "cap " << setw(5) << capacity_
             << "projector:" << (projector_ ? "yes" : "no") << "\n";
    }
};
class Classroom : public Room {
    bool smartboard_;
public:
    Classroom(string id, int cap, bool sb) : Room(move(id), cap), smartboard_(sb) {}
    string typeName() const override { return "Classroom"; }
    bool allows(Purpose p) const override {
        return p == Purpose::Lecture || p == Purpose::Tutorial || p == Purpose::Project;
    }
    void describe() const override {
        cout << left << setw(22) << id_ << setw(17) << typeName() << "cap " << setw(5) << capacity_
             << "smartboard:" << (smartboard_ ? "yes" : "no") << "\n";
    }
};
class Lab : public Room {
    int computers_;
public:
    Lab(string id, int cap, int pcs) : Room(move(id), cap), computers_(pcs) {}
    string typeName() const override { return "Lab"; }
    bool allows(Purpose p) const override { return p == Purpose::Practical || p == Purpose::Project; }
    void describe() const override {
        cout << left << setw(22) << id_ << setw(17) << typeName() << "cap " << setw(5) << capacity_
             << "computers:" << computers_ << "\n";
    }
};
class Auditorium : public Room {
public:
    Auditorium(string id, int cap) : Room(move(id), cap) {}
    string typeName() const override { return "Auditorium"; }
    bool allows(Purpose p) const override { return p == Purpose::Lecture || p == Purpose::Project; }
    void describe() const override {
        cout << left << setw(22) << id_ << setw(17) << typeName() << "cap " << capacity_ << "\n";
    }
};
 
/* Factory: kind = LT | CR | LAB | AUD (default capacities, the timetable has none) */
static Room *makeRoom(const string &kind, const string &id) {
    if (kind == "LT") return new LectureTheatre(id, 120, true);
    if (kind == "CR") return new Classroom(id, 60, false);
    if (kind == "LAB") return new Lab(id, 40, 40);
    return new Auditorium(id, 500);
}
 
/* ---------------- Binary search tree of rooms ---------------- */
class RoomTree {
    struct Node { Room *room; Node *l = nullptr, *r = nullptr; explicit Node(Room *x) : room(x) {} };
    Node *root_ = nullptr;
    static void destroy(Node *n) { if (!n) return; destroy(n->l); destroy(n->r); delete n->room; delete n; }
    static void walk(Node *n, const function<void(Room &)> &fn) {
        if (!n) return;
        walk(n->l, fn); fn(*n->room); walk(n->r, fn);
    }
public:
    RoomTree() = default;
    RoomTree(const RoomTree &) = delete;
    RoomTree &operator=(const RoomTree &) = delete;
    ~RoomTree() { destroy(root_); }
 
    Room *find(const string &id) const {
        Node *n = root_;
        while (n) {
            if (id == n->room->id()) return n->room;
            n = id < n->room->id() ? n->l : n->r;
        }
        return nullptr;
    }
    Room *insert(Room *room) {                    // returns existing room if id already present
        Node **pp = &root_;
        while (*pp) {
            const string &cur = (*pp)->room->id();
            if (room->id() == cur) { delete room; return (*pp)->room; }
            pp = room->id() < cur ? &(*pp)->l : &(*pp)->r;
        }
        *pp = new Node(room);
        return room;
    }
    void inorder(const function<void(Room &)> &fn) const { walk(root_, fn); }
};
 
/* ---------------- The system ---------------- */
class BookingSystem {
    RoomTree rooms_;
    BookingList registry_;
    int nextId_ = 1001;
 
    static vector<string> split(const string &line) {
        vector<string> out; string tok; stringstream ss(line);
        while (getline(ss, tok, ',')) out.push_back(tok);
        if (!line.empty() && line.back() == ',') out.push_back("");
        return out;
    }
    Booking *addBooking(Room &r, const string &user, Purpose p, int strength, int day, int from, int to,
                        bool fixed, int id = 0) {
        Booking *b = new Booking;
        b->user = user; b->purpose = p; b->strength = strength;
        b->day = day; b->from = from; b->to = to; b->fixed = fixed;
        if (!fixed) {
            if (id) { b->id = id; if (id >= nextId_) nextId_ = id + 1; } else b->id = nextId_++;
        }
        r.insert(b);
        if (!fixed) registry_.append(b);
        return b;
    }
    void promoteWaiting(Room &r) {                // earliest request that now fits, repeat until none
        for (;;) {
            WaitRequest *w = r.waiting().extractFirst(
                [&](const WaitRequest &q) { return !r.conflict(q.day, q.from, q.to); });
            if (!w) break;
            Booking *b = addBooking(r, w->user, w->purpose, w->strength, w->day, w->from, w->to, false);
            cout << "  Waiting list: #" << b->id << " confirmed for " << w->user << " (" << r.id() << " "
                 << DAY_NAME[w->day] << " P" << w->from << "-" << w->to << ")\n";
            delete w;
        }
    }
 
public:
    /* ---- file handling ---- */
    bool loadTimetable(const string &path) {
        ifstream in(path);
        if (!in) return false;
        string line;
        getline(in, line);                                    // header
        int rows = 0, booked = 0, merged = 0, skipped = 0;
        while (getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            vector<string> f = split(line);
            if (f.size() < 9) continue;
            rows++;
            int day = stoi(f[2]), from = stoi(f[3]), to = stoi(f[4]);
            Purpose p = f[5] == "Lab" ? Purpose::Practical : Purpose::Lecture;
            Room *r = rooms_.insert(makeRoom(f[8], f[7]));
            if (day < 1 || day > DAYS || from < 1 || to > PERIODS || from > to) { skipped++; continue; }
            Booking *c = r->conflict(day, from, to);
            if (!c) { addBooking(*r, f[6], p, 0, day, from, to, true); booked++; }
            else if (c->fixed && c->user == f[6]) {           // same class shared by several sections
                c->from = min(c->from, from); c->to = max(c->to, to); merged++;
            } else skipped++;
        }
        cout << "Timetable loaded from " << path << ": " << rows << " rows -> " << booked << " slots ("
             << merged << " shared-section rows merged, " << skipped << " skipped)\n";
        return true;
    }
 
    /* bookings.dat: line 1 = next id;  B,room,id,user,purpose,strength,day,from,to
     *                                   W,room,user,purpose,strength,day,from,to */
    void saveBookings(const string &path) const {
        ofstream out(path);
        if (!out) { cout << "  ! Could not write " << path << "\n"; return; }
        out << nextId_ << "\n";
        int nb = 0, nw = 0;
        for (Booking *b = registry_.head; b; b = b->gnext, nb++)
            out << "B," << b->room->id() << ',' << b->id << ',' << b->user << ','
                << static_cast<int>(b->purpose) << ',' << b->strength << ',' << b->day << ','
                << b->from << ',' << b->to << "\n";
        rooms_.inorder([&](Room &r) {
            r.waiting().forEach([&](const WaitRequest &w) {
                out << "W," << r.id() << ',' << w.user << ',' << static_cast<int>(w.purpose) << ','
                    << w.strength << ',' << w.day << ',' << w.from << ',' << w.to << "\n";
                nw++;
            });
        });
        cout << "  Saved " << nb << " bookings and " << nw << " waiting requests to " << path << "\n";
    }
 
    bool loadBookings(const string &path) {
        ifstream in(path);
        if (!in) return false;
        string line;
        int savedNext = 0, nb = 0, nw = 0;
        if (getline(in, line)) savedNext = atoi(line.c_str());
        while (getline(in, line)) {
            vector<string> f = split(line);
            if (f.size() < 8) continue;
            Room *r = rooms_.find(f[1]);
            if (!r) continue;
            if (f[0] == "B" && f.size() >= 9) {
                int day = stoi(f[6]), from = stoi(f[7]), to = stoi(f[8]);
                if (r->conflict(day, from, to)) continue;
                addBooking(*r, f[3], static_cast<Purpose>(stoi(f[4])), stoi(f[5]), day, from, to, false,
                           stoi(f[2]));
                nb++;
            } else if (f[0] == "W") {
                r->waiting().push({f[2], static_cast<Purpose>(stoi(f[3])), stoi(f[4]), stoi(f[5]),
                                   stoi(f[6]), stoi(f[7]), nullptr});
                nw++;
            }
        }
        if (savedNext > nextId_) nextId_ = savedNext;
        cout << "Restored " << nb << " bookings and " << nw << " waiting requests from " << path << "\n";
        return true;
    }
 
    /* ---- operations ---- */
    void book(const string &roomId, const string &user, int purpose, int strength, int day, int from, int to) {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        if (day < 1 || day > DAYS || from < 1 || to > PERIODS || from > to) {
            cout << "  ! Invalid day/period range.\n"; return;
        }
        if (!validPurpose(purpose)) { cout << "  ! Invalid purpose.\n"; return; }
        Purpose p = static_cast<Purpose>(purpose);
        if (!r->allows(p)) {                                             // polymorphic check
            cout << "  ! " << r->typeName() << " cannot host a " << purposeName(p) << ".\n"; return;
        }
        if (strength > r->capacity()) {
            cout << "  ! Strength " << strength << " exceeds capacity " << r->capacity() << ".\n"; return;
        }
        if (Booking *c = r->conflict(day, from, to)) {
            if (c->fixed) cout << "  ! Clash with timetable class " << c->user;
            else cout << "  ! Clash with #" << c->id << " (" << c->user << ")";
            cout << " (periods " << c->from << "-" << c->to << "). Added to waiting list of " << r->id() << ".\n";
            r->waiting().push({user, p, strength, day, from, to, nullptr});
            return;
        }
        Booking *b = addBooking(*r, user, p, strength, day, from, to, false);
        cout << "  Booked #" << b->id << ": " << r->id() << " " << DAY_NAME[day] << " P" << from << "-" << to
             << " (" << PERIOD_TIME[from] << "-" << PERIOD_TIME[to] << ") for " << user << "\n";
    }
 
    void cancel(int id) {
        Booking *b = registry_.find(id);
        if (!b) { cout << "  ! Booking not found.\n"; return; }
        Room &r = *b->room;
        cout << "  Cancelled #" << id << " (" << r.id() << " " << DAY_NAME[b->day] << " P" << b->from << "-"
             << b->to << ", " << b->user << ")\n";
        registry_.unlink(b);
        r.remove(b);
        delete b;
        promoteWaiting(r);
    }
 
    void showDay(const string &roomId, int day) const {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        if (day < 1 || day > DAYS) { cout << "  ! Invalid day.\n"; return; }
        showDay(*r, day);
    }
    void showWeek(const string &roomId) const {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        for (int d = 1; d <= DAYS; d++) showDay(*r, d);
    }
    void showWaiting(const string &roomId) const {
        Room *r = rooms_.find(roomId);
        if (!r) { cout << "  ! No such room.\n"; return; }
        cout << "  Waiting list of " << r->id() << ":\n";
        int n = 0;
        r->waiting().forEach([&](const WaitRequest &w) {
            cout << "    " << ++n << ". " << w.user << "  " << DAY_NAME[w.day] << " P" << w.from << "-"
                 << w.to << "  " << purposeName(w.purpose) << "\n";
        });
        if (!n) cout << "    (empty)\n";
    }
    void listRooms() const { rooms_.inorder([](Room &r) { r.describe(); }); }
 
    void findFree(int day, int from, int to, int purpose, int strength) const {
        if (!validPurpose(purpose) || day < 1 || day > DAYS || from < 1 || to > PERIODS || from > to) {
            cout << "  ! Invalid input.\n"; return;
        }
        Purpose p = static_cast<Purpose>(purpose);
        cout << "  Rooms free on " << DAY_NAME[day] << " P" << from << "-" << to << " for " << purposeName(p)
             << " (" << strength << " students):\n";
        int n = 0;
        rooms_.inorder([&](Room &r) {
            if (r.allows(p) && r.capacity() >= strength && !r.conflict(day, from, to)) {
                cout << "    " << left << setw(22) << r.id() << r.typeName() << " (cap " << r.capacity() << ")\n";
                n++;
            }
        });
        if (!n) cout << "    none\n";
    }
 
private:
    void showDay(const Room &r, int day) const {
        bool used[PERIODS + 2] = {};
        cout << "  " << r.id() << " " << DAY_NAME[day] << ":\n";
        for (Booking *b = r.daySlots(day); b; b = b->next) {
            cout << "    P" << b->from << "-" << b->to << " (" << PERIOD_TIME[b->from] << ")  ";
            if (b->fixed) cout << "TT    " << left << setw(10) << purposeName(b->purpose) << b->user << " (timetable)\n";
            else cout << "#" << b->id << "  " << left << setw(10) << purposeName(b->purpose) << b->user << ", "
                      << b->strength << " students\n";
            for (int i = b->from; i <= b->to; i++) used[i] = true;
        }
        cout << "    Free periods:";
        bool any = false;
        for (int i = 1; i <= PERIODS; i++) if (!used[i]) { cout << " " << i; any = true; }
        cout << (any ? "" : " none") << "\n";
    }
};
 
/* ---------------- Console UI ---------------- */
static bool readInt(const string &q, int &v) { cout << q; return static_cast<bool>(cin >> v); }
static bool readStr(const string &q, string &s) { cout << q; return static_cast<bool>(cin >> s); }
 
int main() {
    BookingSystem sys;
    if (!sys.loadTimetable("timetable.csv")) {
        cout << "Could not open timetable.csv (export it from the Excel file with export_csv.py).\n";
        return 1;
    }
    sys.loadBookings("bookings.dat");
    cout << "=== Classroom & Lab Slot Booking ===\nDays: 1=Mon..6=Sat   Periods: 1..10 (08:00-17:50)\n"
            "Purpose: 1=Lecture 2=Tutorial 3=Practical 4=Project\n";
    for (;;) {
        cout << "\n1 Book  2 Cancel  3 Room day view  4 Room week view  5 Find free rooms\n"
                "6 Waiting list  7 List rooms  8 Save  0 Exit\n> ";
        int ch;
        if (!(cin >> ch)) break;
        string rid, user; int p, s, d, f, t;
        bool ok = true;
        switch (ch) {
        case 1:
            ok = readStr("Room id: ", rid) && readStr("Booked by (no spaces): ", user) &&
                 readInt("Purpose: ", p) && readInt("Strength: ", s) && readInt("Day: ", d) &&
                 readInt("From period: ", f) && readInt("To period: ", t);
            if (ok) sys.book(rid, user, p, s, d, f, t);
            break;
        case 2: if ((ok = readInt("Booking id: ", d))) sys.cancel(d); break;
        case 3: if ((ok = readStr("Room id: ", rid) && readInt("Day: ", d))) sys.showDay(rid, d); break;
        case 4: if ((ok = readStr("Room id: ", rid))) sys.showWeek(rid); break;
        case 5:
            ok = readInt("Day: ", d) && readInt("From: ", f) && readInt("To: ", t) &&
                 readInt("Purpose: ", p) && readInt("Strength: ", s);
            if (ok) sys.findFree(d, f, t, p, s);
            break;
        case 6: if ((ok = readStr("Room id: ", rid))) sys.showWaiting(rid); break;
        case 7: sys.listRooms(); break;
        case 8: sys.saveBookings("bookings.dat"); break;
        case 0: sys.saveBookings("bookings.dat"); return 0;
        default: cout << "  ! Invalid choice.\n";
        }
        if (!ok) break;                                   // input ended
    }
    sys.saveBookings("bookings.dat");
    return 0;
}