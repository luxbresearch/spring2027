#include "err.h"
#include <iostream>

void margaret1::margaretError(std::string source, std::string message, std::string solve) {
    std::cout << "[!!!] Margaret 1 encountered an unrecoverable error" << std::endl;
    std::cout << " -->  From:\t" << source << std::endl;
    std::cout << " -->  Error:\t" << message << std::endl;
    std::cout << " -->  Try:\t" << solve << std::endl;
    std::cout << "[!!!] Program cannot progress :(" << std::endl;
    throw std::runtime_error("Unrecoverable Margaret error");
}