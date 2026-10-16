Part 4: viewing a day, listing rooms and the menu (lines 155–203)

What it does: this part finishes BookingSystem and adds the screen the user sees.

showDay(): it prints every booking for one room on one day, then shows which periods are still free.
listRooms(): it walks the tree in order and asks each room to describe itself, so rooms print sorted by id.
Input helpers: readInt and readStr print a question and read the answer. They report failure if the input ends, so the program can stop cleanly.
main(): it creates the system, then loops on a menu:
1: book a slot.
2: view a room’s day.
3: list rooms.
0: exit.
cpp
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
