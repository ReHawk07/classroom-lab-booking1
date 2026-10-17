#include "RoomTypes.h"

#include <utility>

LectureTheatre::LectureTheatre(std::string id, int capacity)
    : Room(std::move(id), capacity) {}

std::string LectureTheatre::typeName() const {
    return "Lecture Theatre";
}

bool LectureTheatre::allows(Purpose purpose) const {
    return purpose == Purpose::Lecture;
}

Classroom::Classroom(std::string id, int capacity)
    : Room(std::move(id), capacity) {}

std::string Classroom::typeName() const {
    return "Classroom";
}

bool Classroom::allows(Purpose purpose) const {
    return purpose != Purpose::Practical;
}

Lab::Lab(std::string id, int capacity)
    : Room(std::move(id), capacity) {}

std::string Lab::typeName() const {
    return "Lab";
}

bool Lab::allows(Purpose purpose) const {
    return purpose == Purpose::Practical || purpose == Purpose::Project;
}
