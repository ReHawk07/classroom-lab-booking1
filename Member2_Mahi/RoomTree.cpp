#include "RoomTree.h"

struct RoomTree::Node {
    Room* room;
    Node* left = nullptr;
    Node* right = nullptr;

    explicit Node(Room* value) : room(value) {}
};

RoomTree::~RoomTree() {
    destroy(root_);
}

void RoomTree::destroy(Node* node) {
    if (!node) {
        return;
    }
    destroy(node->left);
    destroy(node->right);
    delete node->room;
    delete node;
}

void RoomTree::walk(Node* node, const std::function<void(Room&)>& callback) {
    if (!node) {
        return;
    }
    walk(node->left, callback);
    callback(*node->room);
    walk(node->right, callback);
}

Room* RoomTree::find(const std::string& id) const {
    Node* node = root_;
    while (node) {
        if (id == node->room->id()) {
            return node->room;
        }
        node = id < node->room->id() ? node->left : node->right;
    }
    return nullptr;
}

void RoomTree::insert(Room* room) {
    if (!room) {
        return;
    }

    Node** position = &root_;
    while (*position) {
        if (room->id() == (*position)->room->id()) {
            delete room; // Duplicate IDs are ignored; tree retains ownership of existing room.
            return;
        }
        position = room->id() < (*position)->room->id()
            ? &(*position)->left
            : &(*position)->right;
    }
    *position = new Node(room);
}

void RoomTree::inorder(const std::function<void(Room&)>& callback) const {
    walk(root_, callback);
}
