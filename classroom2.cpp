Part 2: the room types and the room tree (lines 65–115)

What it does: this part adds the real room types and the structure that stores all rooms.

Three room types: LectureTheatre, Classroom and Lab each extend Room. They differ only in what allows() says:
A lecture theatre accepts only lectures.
A classroom accepts anything except practicals.
A lab accepts practicals and projects.
Why this is useful: the rest of the program calls room->allows(...) without knowing which type it has. Each room answers by its own rules. This is called polymorphism.
RoomTree: a binary search tree that stores rooms by their id. Smaller ids go left and bigger ids go right.
find() follows left or right until it reaches the room, which is quick.
insert() adds a room in the right place and ignores a duplicate id.
inorder() visits the rooms in sorted order, which is how the room list prints neatly.
cpp
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