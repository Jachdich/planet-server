#include "logging.h"

#include <iostream>
#include <string>

Logger::Logger() {}
void Logger::warn(std::string msg) {
    std::cout << msg << "\n";
}

void Logger::error(std::string msg) {
    std::cerr << "ERROR: " << msg << "\n";
}
