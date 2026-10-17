#include "ConsoleUI.h"

#include <iostream>

bool readInt(const std::string& prompt, int& value) {
    std::cout << prompt;
    return static_cast<bool>(std::cin >> value);
}

bool readStr(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    return static_cast<bool>(std::cin >> value);
}
