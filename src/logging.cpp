#include "logging.h"

#include <iostream>
#include <string>

#define LOGGING_DEBUG true

Logger::Logger() {}
void Logger::warn(std::string msg) {
    std::cout << "[WARN ]" << msg << "\n";
}

void Logger::error(std::string msg) {
    std::cerr << "[ERROR] " << msg << "\n";
}

void Logger::info(std::string msg) {
    std::cout << "[INFO ] " << msg << "\n";
}

void Logger::debug(std::string msg) {
    if (LOGGING_DEBUG) {
        std::cout << "[DEBUG] " << msg << "\n";
    }
}
