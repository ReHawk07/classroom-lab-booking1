Part 3: the booking system and the book() function (lines 116–154)

What it does: this part holds the main logic, which is making a booking. BookingSystem is the class that ties the tree of rooms and the bookings together. This part opens the class and contains its first two members; Part 4 finishes it.

Constructor: it adds seven sample rooms when the program starts.
book(): it checks the request step by step and stops with a clear message at the first problem:
Does the room exist?
Are the day, periods and purpose valid?
Does this room type allow this purpose? For example, a lab refuses a lecture.
Does the class fit the room’s capacity?
Does the time clash with an existing booking?
If all checks pass: it creates a Booking with a new id, puts it into the room’s sorted list for that day, and prints a confirmation.
cpp

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
