#ifndef ROOM_TREE_H
#define ROOM_TREE_H

#include "Room.h"
#include <functional>
#include <string>

// Owns rooms and stores them by ID in a binary search tree.
class RoomTree {
private:
    struct Node;
    Node* root_ = nullptr;

    static void destroy(Node* node);
    static void walk(Node* node, const std::function<void(Room&)>& callback);

public:
    RoomTree() = default;
    ~RoomTree();

    RoomTree(const RoomTree&) = delete;
    RoomTree& operator=(const RoomTree&) = delete;

    Room* find(const std::string& id) const;
    void insert(Room* room);
    void inorder(const std::function<void(Room&)>& callback) const;
};

#endif // ROOM_TREE_H
