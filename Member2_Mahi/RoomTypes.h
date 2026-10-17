#ifndef ROOM_TYPES_H
#define ROOM_TYPES_H

#include "Room.h"

class LectureTheatre : public Room {
public:
    LectureTheatre(std::string id, int capacity);
    std::string typeName() const override;
    bool allows(Purpose purpose) const override;
};

class Classroom : public Room {
public:
    Classroom(std::string id, int capacity);
    std::string typeName() const override;
    bool allows(Purpose purpose) const override;
};

class Lab : public Room {
public:
    Lab(std::string id, int capacity);
    std::string typeName() const override;
    bool allows(Purpose purpose) const override;
};

#endif // ROOM_TYPES_H
